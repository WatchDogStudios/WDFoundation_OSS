#include <FileservePlugin/FileservePluginPCH.h>

#include <FileservePlugin/Client/FileserveClient.h>
#include <FileservePlugin/Fileserver/ClientContext.h>
#include <Foundation/Communication/GlobalEvent.h>
#include <Foundation/Communication/RemoteInterfaceEnet.h>
#include <Foundation/IO/FileSystem/FileWriter.h>
#include <Foundation/IO/FileSystem/Implementation/DataDirType.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Types/ScopeExit.h>
#include <Foundation/Utilities/CommandLineUtils.h>

NS_IMPLEMENT_SINGLETON(nsFileserveClient);

bool nsFileserveClient::s_bEnableFileserve = true;

nsFileserveClient::nsFileserveClient()
  : m_SingletonRegistrar(this)
{
  AddServerAddressToTry("localhost:1042");

  nsStringBuilder sAddress, sSearch;

  // the app directory
  {
    sSearch = nsOSFile::GetApplicationDirectory();
    sSearch.AppendPath("nsFileserve.txt");

    if (TryReadFileserveConfig(sSearch, sAddress).Succeeded())
    {
      AddServerAddressToTry(sAddress);
    }
  }

  // command line argument
  AddServerAddressToTry(nsCommandLineUtils::GetGlobalInstance()->GetStringOption("-fs_server", 0, ""));

  // last successful IP is stored in the user directory
  {
    sSearch = nsOSFile::GetUserDataFolder("nsFileserve.txt");

    if (TryReadFileserveConfig(sSearch, sAddress).Succeeded())
    {
      AddServerAddressToTry(sAddress);
    }
  }

  if (nsCommandLineUtils::GetGlobalInstance()->GetBoolOption("-fs_off"))
    s_bEnableFileserve = false;

  m_CurrentTime = nsTime::Now();
}

nsFileserveClient::~nsFileserveClient()
{
  ShutdownConnection();
}

void nsFileserveClient::ShutdownConnection()
{
  if (m_pNetwork)
  {
    nsLog::Dev("Shutting down fileserve client");

    m_pNetwork->ShutdownConnection();
    m_pNetwork = nullptr;
  }
}

void nsFileserveClient::ClearState()
{
  m_bDownloading = false;
  m_bWaitingForUploadFinished = false;
  m_CurFileRequestGuid = nsUuid();
  m_sCurFileRequest.Clear();
  m_Download.Clear();
}

nsResult nsFileserveClient::EnsureConnected(nsTime timeout)
{
  NS_LOCK(m_Mutex);
  if (!s_bEnableFileserve || m_bFailedToConnect)
    return NS_FAILURE;

  if (m_pNetwork == nullptr)
  {
    m_pNetwork = nsRemoteInterfaceEnet::Make(); /// \todo Somehow abstract this away ?

    m_sFileserveCacheFolder = nsOSFile::GetUserDataFolder("nsFileserve/Cache");
    m_sFileserveCacheMetaFolder = nsOSFile::GetUserDataFolder("nsFileserve/Meta");

    if (nsOSFile::CreateDirectoryStructure(m_sFileserveCacheFolder).Failed())
    {
      nsLog::Error("Could not create fileserve cache folder '{0}'", m_sFileserveCacheFolder);
      return NS_FAILURE;
    }

    if (nsOSFile::CreateDirectoryStructure(m_sFileserveCacheMetaFolder).Failed())
    {
      nsLog::Error("Could not create fileserve cache folder '{0}'", m_sFileserveCacheMetaFolder);
      return NS_FAILURE;
    }
  }

  if (!m_pNetwork->IsConnectedToServer())
  {
    ClearState();
    m_bFailedToConnect = true;

    if (m_pNetwork->ConnectToServer('NSFS', m_sServerConnectionAddress).Failed())
      return NS_FAILURE;

    if (timeout.GetSeconds() < 0)
    {
      timeout = nsTime::MakeFromSeconds(nsCommandLineUtils::GetGlobalInstance()->GetFloatOption("-fs_timeout", -timeout.GetSeconds()));
    }

    if (m_pNetwork->WaitForConnectionToServer(timeout).Failed())
    {
      m_pNetwork->ShutdownConnection();
      nsLog::Error("Connection to nsFileserver timed out");
      return NS_FAILURE;
    }
    else
    {
      nsLog::Success("Connected to nsFileserver '{0}", m_sServerConnectionAddress);
      m_pNetwork->SetMessageHandler('FSRV', nsMakeDelegate(&nsFileserveClient::NetworkMsgHandler, this));

      m_pNetwork->Send('FSRV', 'HELO'); // be friendly
    }

    m_bFailedToConnect = false;
  }

  return NS_SUCCESS;
}

void nsFileserveClient::UpdateClient()
{
  NS_LOCK(m_Mutex);
  if (m_pNetwork == nullptr || m_bFailedToConnect || !s_bEnableFileserve)
    return;

  if (!m_pNetwork->IsConnectedToServer())
  {
    if (EnsureConnected().Failed())
    {
      nsLog::Error("Fileserve connection was lost and could not be re-established.");
      ShutdownConnection();
    }
    return;
  }

  m_CurrentTime = nsTime::Now();

  m_pNetwork->ExecuteAllMessageHandlers();
}

void nsFileserveClient::AddServerAddressToTry(nsStringView sAddress)
{
  NS_LOCK(m_Mutex);
  if (sAddress.IsEmpty())
    return;

  if (m_TryServerAddresses.Contains(sAddress))
    return;

  m_TryServerAddresses.PushBack(sAddress);

  // always set the most recent address as the default one
  m_sServerConnectionAddress = sAddress;
}

void nsFileserveClient::UploadFile(nsUInt16 uiDataDirID, const char* szFile, const nsDynamicArray<nsUInt8>& fileContent)
{
  NS_LOCK(m_Mutex);

  if (m_pNetwork == nullptr)
    return;

  // update meta state and cache
  {
    const nsString& sMountPoint = m_MountedDataDirs[uiDataDirID].m_sMountPoint;
    nsStringBuilder sCachedMetaFile;
    BuildPathInCache(szFile, sMountPoint, nullptr, &sCachedMetaFile);

    nsUInt64 uiHash = 1;

    if (!fileContent.IsEmpty())
    {
      uiHash = nsHashingUtils::xxHash64(fileContent.GetData(), fileContent.GetCount(), uiHash);
    }

    WriteMetaFile(sCachedMetaFile, 0, uiHash);

    InvalidateFileCache(uiDataDirID, szFile, uiHash);
  }

  const nsUInt32 uiFileSize = fileContent.GetCount();

  nsUuid uploadGuid = nsUuid::MakeUuid();

  {
    nsRemoteMessage msg;
    msg.SetMessageID('FSRV', 'UPLH');
    msg.GetWriter() << uploadGuid;
    msg.GetWriter() << uiFileSize;
    msg.GetWriter() << uiDataDirID;
    msg.GetWriter() << szFile;
    m_pNetwork->Send(nsRemoteTransmitMode::Reliable, msg);
  }

  nsUInt32 uiNextByte = 0;

  // send the file over in multiple packages of 1KB each
  // send at least one package, even for empty files

  while (uiNextByte < fileContent.GetCount())
  {
    const nsUInt16 uiChunkSize = (nsUInt16)nsMath::Min<nsUInt32>(1024, fileContent.GetCount() - uiNextByte);

    nsRemoteMessage msg;
    msg.GetWriter() << uploadGuid;
    msg.GetWriter() << uiChunkSize;
    msg.GetWriter().WriteBytes(&fileContent[uiNextByte], uiChunkSize).IgnoreResult();

    msg.SetMessageID('FSRV', 'UPLD');
    m_pNetwork->Send(nsRemoteTransmitMode::Reliable, msg);

    uiNextByte += uiChunkSize;
  }

  // continuously update the network until we know the server has received the big chunk of data
  m_bWaitingForUploadFinished = true;

  // final message to server
  {
    nsRemoteMessage msg('FSRV', 'UPLF');
    msg.GetWriter() << uploadGuid;
    msg.GetWriter() << uiDataDirID;
    msg.GetWriter() << szFile;

    m_pNetwork->Send(nsRemoteTransmitMode::Reliable, msg);
  }

  while (m_bWaitingForUploadFinished)
  {
    UpdateClient();
  }
}


void nsFileserveClient::InvalidateFileCache(nsUInt16 uiDataDirID, nsStringView sFile, nsUInt64 uiHash)
{
  NS_LOCK(m_Mutex);
  auto& cache = m_MountedDataDirs[uiDataDirID].m_CacheStatus[sFile];
  cache.m_FileHash = uiHash;
  cache.m_TimeStamp = 0;
  cache.m_LastCheck = nsTime::MakeZero(); // will trigger a server request and that in turn will update the file timestamp

  // redirect the next access to this cache entry
  // together with the zero LastCheck that will make sure the best match gets updated as well
  m_FileDataDir[sFile] = uiDataDirID;
}

void nsFileserveClient::FillFileStatusCache(const char* szFile)
{
  NS_LOCK(m_Mutex);
  auto it = m_FileDataDir.FindOrAdd(szFile);
  it.Value() = 0xffff; // does not exist

  for (nsUInt16 i = static_cast<nsUInt16>(m_MountedDataDirs.GetCount()); i > 0; --i)
  {
    const nsUInt16 dd = i - 1;

    if (!m_MountedDataDirs[dd].m_bMounted)
      continue;

    auto& cache = m_MountedDataDirs[dd].m_CacheStatus[szFile];

    DetermineCacheStatus(dd, szFile, cache);
    cache.m_LastCheck = nsTime::MakeZero();

    if (cache.m_TimeStamp != 0 && cache.m_FileHash != 0) // file exists
    {
      // best possible candidate
      if (it.Value() == 0xffff)
        it.Value() = dd;
    }
  }

  if (it.Value() == 0xffff)
    it.Value() = 0; // fallback
}

void nsFileserveClient::BuildPathInCache(const char* szFile, const char* szMountPoint, nsStringBuilder* out_pAbsPath, nsStringBuilder* out_pFullPathMeta) const
{
  NS_ASSERT_DEV(!nsPathUtils::IsAbsolutePath(szFile), "Invalid path");
  NS_LOCK(m_Mutex);
  if (out_pAbsPath)
  {
    *out_pAbsPath = m_sFileserveCacheFolder;
    out_pAbsPath->AppendPath(szMountPoint, szFile);
    out_pAbsPath->MakeCleanPath();
  }
  if (out_pFullPathMeta)
  {
    *out_pFullPathMeta = m_sFileserveCacheMetaFolder;
    out_pFullPathMeta->AppendPath(szMountPoint, szFile);
    out_pFullPathMeta->MakeCleanPath();
  }
}

void nsFileserveClient::ComputeDataDirMountPoint(nsStringView sDataDir, nsStringBuilder& out_sMountPoint)
{
  NS_ASSERT_DEV(sDataDir.IsEmpty() || sDataDir.EndsWith("/"), "Invalid path");

  const nsUInt32 uiMountPoint = nsHashingUtils::xxHash32String(sDataDir);
  out_sMountPoint.SetFormat("{0}", nsArgU(uiMountPoint, 8, true, 16));
}

void nsFileserveClient::GetFullDataDirCachePath(const char* szDataDir, nsStringBuilder& out_sFullPath, nsStringBuilder& out_sFullPathMeta) const
{
  NS_LOCK(m_Mutex);
  nsStringBuilder sMountPoint;
  ComputeDataDirMountPoint(szDataDir, sMountPoint);

  out_sFullPath = m_sFileserveCacheFolder;
  out_sFullPath.AppendPath(sMountPoint);

  out_sFullPathMeta = m_sFileserveCacheMetaFolder;
  out_sFullPathMeta.AppendPath(sMountPoint);
}

void nsFileserveClient::NetworkMsgHandler(nsRemoteMessage& msg)
{
  NS_LOCK(m_Mutex);
  if (msg.GetMessageID() == 'DWNL')
  {
    HandleFileTransferMsg(msg);
    return;
  }

  if (msg.GetMessageID() == 'DWNF')
  {
    HandleFileTransferFinishedMsg(msg);
    return;
  }

  static bool s_bReloadResources = false;

  if (msg.GetMessageID() == 'RLDR')
  {
    s_bReloadResources = true;
  }

  if (!m_bDownloading && s_bReloadResources)
  {
    NS_BROADCAST_EVENT(nsResourceManager_ReloadAllResources);
    s_bReloadResources = false;
    return;
  }

  if (msg.GetMessageID() == 'RLDR')
    return;

  if (msg.GetMessageID() == 'UACK')
  {
    m_bWaitingForUploadFinished = false;
    return;
  }

  nsLog::Error("Unknown FSRV message: '{0}' - {1} bytes", msg.GetMessageID(), msg.GetMessageData().GetCount());
}

nsUInt16 nsFileserveClient::MountDataDirectory(nsStringView sDataDirectory, nsStringView sRootName)
{
  NS_LOCK(m_Mutex);
  if (!m_pNetwork->IsConnectedToServer())
    return 0xffff;

  nsStringBuilder sRoot = sRootName;
  sRoot.Trim(":/");

  nsStringBuilder sMountPoint;
  ComputeDataDirMountPoint(sDataDirectory, sMountPoint);

  const nsUInt16 uiDataDirID = static_cast<nsUInt16>(m_MountedDataDirs.GetCount());

  nsRemoteMessage msg('FSRV', ' MNT');
  msg.GetWriter() << sDataDirectory;
  msg.GetWriter() << sRoot;
  msg.GetWriter() << sMountPoint;
  msg.GetWriter() << uiDataDirID;

  m_pNetwork->Send(nsRemoteTransmitMode::Reliable, msg);

  auto& dd = m_MountedDataDirs.ExpandAndGetRef();
  // dd.m_sPathOnClient = sDataDirectory;
  // dd.m_sRootName = sRoot;
  dd.m_sMountPoint = sMountPoint;
  dd.m_bMounted = true;

  return uiDataDirID;
}


void nsFileserveClient::UnmountDataDirectory(nsUInt16 uiDataDir)
{
  NS_LOCK(m_Mutex);
  if (!m_pNetwork->IsConnectedToServer())
    return;

  nsRemoteMessage msg('FSRV', 'UMNT');
  msg.GetWriter() << uiDataDir;

  m_pNetwork->Send(nsRemoteTransmitMode::Reliable, msg);

  auto& dd = m_MountedDataDirs[uiDataDir];
  dd.m_bMounted = false;
}

void nsFileserveClient::DeleteFile(nsUInt16 uiDataDir, nsStringView sFile)
{
  NS_LOCK(m_Mutex);
  if (!m_pNetwork->IsConnectedToServer())
    return;

  InvalidateFileCache(uiDataDir, sFile, 0);

  nsRemoteMessage msg('FSRV', 'DELF');
  msg.GetWriter() << uiDataDir;
  msg.GetWriter() << sFile;

  m_pNetwork->Send(nsRemoteTransmitMode::Reliable, msg);
}

void nsFileserveClient::HandleFileTransferMsg(nsRemoteMessage& msg)
{
  NS_LOCK(m_Mutex);
  {
    nsUuid fileRequestGuid;
    msg.GetReader() >> fileRequestGuid;

    if (fileRequestGuid != m_CurFileRequestGuid)
    {
      // nsLog::Debug("Fileserver is answering someone else");
      return;
    }
  }

  nsUInt16 uiChunkSize = 0;
  msg.GetReader() >> uiChunkSize;

  nsUInt32 uiFileSize = 0;
  msg.GetReader() >> uiFileSize;

  // make sure we don't need to reallocate
  m_Download.Reserve(uiFileSize);

  if (uiChunkSize > 0)
  {
    const nsUInt32 uiStartPos = m_Download.GetCount();
    m_Download.SetCountUninitialized(uiStartPos + uiChunkSize);
    msg.GetReader().ReadBytes(&m_Download[uiStartPos], uiChunkSize);
  }
}


void nsFileserveClient::HandleFileTransferFinishedMsg(nsRemoteMessage& msg)
{
  NS_LOCK(m_Mutex);
  NS_SCOPE_EXIT(m_bDownloading = false);

  {
    nsUuid fileRequestGuid;
    msg.GetReader() >> fileRequestGuid;

    if (fileRequestGuid != m_CurFileRequestGuid)
    {
      // nsLog::Debug("Fileserver is answering someone else");
      return;
    }
  }

  nsFileserveFileState fileState;
  {
    nsInt8 iFileStatus = 0;
    msg.GetReader() >> iFileStatus;
    fileState = (nsFileserveFileState)iFileStatus;
  }

  nsInt64 iFileTimeStamp = 0;
  msg.GetReader() >> iFileTimeStamp;

  nsUInt64 uiFileHash = 0;
  msg.GetReader() >> uiFileHash;

  nsUInt16 uiFoundInDataDir = 0;
  msg.GetReader() >> uiFoundInDataDir;

  if (uiFoundInDataDir == 0xffff)         // file does not exist on server in any data dir
  {
    m_FileDataDir[m_sCurFileRequest] = 0; // placeholder

    for (nsUInt32 i = 0; i < m_MountedDataDirs.GetCount(); ++i)
    {
      auto& ref = m_MountedDataDirs[i].m_CacheStatus[m_sCurFileRequest];
      ref.m_FileHash = 0;
      ref.m_TimeStamp = 0;
      ref.m_LastCheck = m_CurrentTime;
    }

    return;
  }
  else
  {
    m_FileDataDir[m_sCurFileRequest] = uiFoundInDataDir;

    auto& ref = m_MountedDataDirs[uiFoundInDataDir].m_CacheStatus[m_sCurFileRequest];
    ref.m_FileHash = uiFileHash;
    ref.m_TimeStamp = iFileTimeStamp;
    ref.m_LastCheck = m_CurrentTime;
  }

  // nothing changed
  if (fileState == nsFileserveFileState::SameTimestamp || fileState == nsFileserveFileState::NonExistantEither)
    return;

  const nsString& sMountPoint = m_MountedDataDirs[uiFoundInDataDir].m_sMountPoint;
  nsStringBuilder sCachedFile, sCachedMetaFile;
  BuildPathInCache(m_sCurFileRequest, sMountPoint, &sCachedFile, &sCachedMetaFile);

  if (fileState == nsFileserveFileState::NonExistant)
  {
    // remove them from the cache as well, if they still exist there
    nsOSFile::DeleteFile(sCachedFile).IgnoreResult();
    nsOSFile::DeleteFile(sCachedMetaFile).IgnoreResult();
    return;
  }

  // timestamp changed, but hash is still the same -> update timestamp
  if (fileState == nsFileserveFileState::SameHash)
  {
    WriteMetaFile(sCachedMetaFile, iFileTimeStamp, uiFileHash);
  }

  if (fileState == nsFileserveFileState::Different)
  {
    WriteDownloadToDisk(sCachedFile);
    WriteMetaFile(sCachedMetaFile, iFileTimeStamp, uiFileHash);
  }
}


void nsFileserveClient::WriteMetaFile(nsStringBuilder sCachedMetaFile, nsInt64 iFileTimeStamp, nsUInt64 uiFileHash)
{
  nsOSFile file;
  if (file.Open(sCachedMetaFile, nsFileOpenMode::Write).Succeeded())
  {
    file.Write(&iFileTimeStamp, sizeof(nsInt64)).IgnoreResult();
    file.Write(&uiFileHash, sizeof(nsUInt64)).IgnoreResult();

    file.Close();
  }
  else
  {
    nsLog::Error("Failed to write meta file to '{0}'", sCachedMetaFile);
  }
}

void nsFileserveClient::WriteDownloadToDisk(nsStringBuilder sCachedFile)
{
  NS_LOCK(m_Mutex);
  nsOSFile file;
  if (file.Open(sCachedFile, nsFileOpenMode::Write).Succeeded())
  {
    if (!m_Download.IsEmpty())
      file.Write(m_Download.GetData(), m_Download.GetCount()).IgnoreResult();

    file.Close();
  }
  else
  {
    nsLog::Error("Failed to write download to '{0}'", sCachedFile);
  }
}

nsResult nsFileserveClient::DownloadFile(nsUInt16 uiDataDirID, const char* szFile, bool bForceThisDataDir, nsStringBuilder* out_pFullPath)
{
  // bForceThisDataDir = true;
  NS_LOCK(m_Mutex);
  if (m_bDownloading)
  {
    nsLog::Warning("Trying to download a file over fileserve while another file is already downloading. Recursive download is ignored.");
    return NS_FAILURE;
  }

  NS_ASSERT_DEV(uiDataDirID < m_MountedDataDirs.GetCount(), "Invalid data dir index {0}", uiDataDirID);
  NS_ASSERT_DEV(m_MountedDataDirs[uiDataDirID].m_bMounted, "Data directory {0} is not mounted", uiDataDirID);
  NS_ASSERT_DEV(!m_bDownloading, "Cannot start a download, while one is still running");

  if (!m_pNetwork->IsConnectedToServer())
    return NS_FAILURE;

  bool bCachedYet = false;
  auto itFileDataDir = m_FileDataDir.FindOrAdd(szFile, &bCachedYet);
  if (!bCachedYet)
  {
    FillFileStatusCache(szFile);
  }

  const nsUInt16 uiUseDataDirCache = bForceThisDataDir ? uiDataDirID : itFileDataDir.Value();
  const FileCacheStatus& CacheStatus = m_MountedDataDirs[uiUseDataDirCache].m_CacheStatus[szFile];

  if (m_CurrentTime - CacheStatus.m_LastCheck < nsTime::MakeFromSeconds(5.0f))
  {
    if (CacheStatus.m_FileHash == 0) // file does not exist
      return NS_FAILURE;

    if (out_pFullPath)
      BuildPathInCache(szFile, m_MountedDataDirs[uiUseDataDirCache].m_sMountPoint, out_pFullPath, nullptr);

    return NS_SUCCESS;
  }

  m_Download.Clear();
  m_sCurFileRequest = szFile;
  m_CurFileRequestGuid = nsUuid::MakeUuid();
  m_bDownloading = true;

  nsRemoteMessage msg('FSRV', 'READ');
  msg.GetWriter() << uiUseDataDirCache;
  msg.GetWriter() << bForceThisDataDir;
  msg.GetWriter() << szFile;
  msg.GetWriter() << m_CurFileRequestGuid;
  msg.GetWriter() << CacheStatus.m_TimeStamp;
  msg.GetWriter() << CacheStatus.m_FileHash;

  m_pNetwork->Send(nsRemoteTransmitMode::Reliable, msg);

  while (m_bDownloading)
  {
    m_pNetwork->UpdateRemoteInterface();
    m_pNetwork->ExecuteAllMessageHandlers();
  }

  if (bForceThisDataDir)
  {
    if (m_MountedDataDirs[uiDataDirID].m_CacheStatus[m_sCurFileRequest].m_FileHash == 0)
      return NS_FAILURE;

    if (out_pFullPath)
      BuildPathInCache(szFile, m_MountedDataDirs[uiDataDirID].m_sMountPoint, out_pFullPath, nullptr);

    return NS_SUCCESS;
  }
  else
  {
    const nsUInt16 uiBestDir = itFileDataDir.Value();
    if (uiBestDir == uiDataDirID) // best match is still this? -> success
    {
      // file does not exist
      if (m_MountedDataDirs[uiBestDir].m_CacheStatus[m_sCurFileRequest].m_FileHash == 0)
        return NS_FAILURE;

      if (out_pFullPath)
        BuildPathInCache(szFile, m_MountedDataDirs[uiBestDir].m_sMountPoint, out_pFullPath, nullptr);

      return NS_SUCCESS;
    }

    return NS_FAILURE;
  }
}

void nsFileserveClient::DetermineCacheStatus(nsUInt16 uiDataDirID, const char* szFile, FileCacheStatus& out_Status) const
{
  NS_LOCK(m_Mutex);
  nsStringBuilder sAbsPathFile, sAbsPathMeta;
  const auto& dd = m_MountedDataDirs[uiDataDirID];

  NS_ASSERT_DEV(dd.m_bMounted, "Data directory {0} is not mounted", uiDataDirID);

  BuildPathInCache(szFile, dd.m_sMountPoint, &sAbsPathFile, &sAbsPathMeta);

  if (nsOSFile::ExistsFile(sAbsPathFile))
  {
    nsOSFile meta;
    if (meta.Open(sAbsPathMeta, nsFileOpenMode::Read).Failed())
    {
      // cleanup, when the meta file does not exist, the data file is useless
      nsOSFile::DeleteFile(sAbsPathFile).IgnoreResult();
      return;
    }

    meta.Read(&out_Status.m_TimeStamp, sizeof(nsInt64));
    meta.Read(&out_Status.m_FileHash, sizeof(nsUInt64));
  }
}

nsResult nsFileserveClient::TryReadFileserveConfig(const char* szFile, nsStringBuilder& out_Result)
{
  nsOSFile file;
  if (file.Open(szFile, nsFileOpenMode::Read).Succeeded())
  {
    nsUInt8 data[64]; // an IP + port should not be longer than 22 characters

    nsStringBuilder res;

    data[file.Read(data, 63)] = 0;
    res = (const char*)data;
    res.Trim(" \t\n\r");

    if (res.IsEmpty())
      return NS_FAILURE;

    // has to contain a port number
    if (res.FindSubString(":") == nullptr)
      return NS_FAILURE;

    // otherwise could be an arbitrary string
    out_Result = res;
    return NS_SUCCESS;
  }

  return NS_FAILURE;
}

nsResult nsFileserveClient::SearchForServerAddress(nsTime timeout /*= nsTime::MakeFromSeconds(5)*/)
{
  NS_LOCK(m_Mutex);
  if (!s_bEnableFileserve)
    return NS_FAILURE;

  nsStringBuilder sAddress;

  // add the command line argument again, in case this was modified since the constructor ran
  // will not change anything, if this is a duplicate
  AddServerAddressToTry(nsCommandLineUtils::GetGlobalInstance()->GetStringOption("-fs_server", 0, ""));

  // go through the available options
  for (nsInt32 idx = m_TryServerAddresses.GetCount() - 1; idx >= 0; --idx)
  {
    if (TryConnectWithFileserver(m_TryServerAddresses[idx], timeout).Succeeded())
      return NS_SUCCESS;
  }

  return NS_FAILURE;
}

nsResult nsFileserveClient::TryConnectWithFileserver(const char* szAddress, nsTime timeout) const
{
  NS_LOCK(m_Mutex);
  if (nsStringUtils::IsNullOrEmpty(szAddress))
    return NS_FAILURE;

  nsLog::Info("File server address: '{0}' ({1} sec)", szAddress, timeout.GetSeconds());

  nsUniquePtr<nsRemoteInterfaceEnet> network = nsRemoteInterfaceEnet::Make(); /// \todo Abstract this somehow ?
  if (network->ConnectToServer('NSFS', szAddress, false).Failed())
    return NS_FAILURE;

  bool bServerFound = false;
  network->SetMessageHandler('FSRV', [&bServerFound](nsRemoteMessage& ref_msg)
    {
    switch (ref_msg.GetMessageID())
    {
      case ' YES':
        bServerFound = true;
        break;
    } });

  if (network->WaitForConnectionToServer(timeout).Succeeded())
  {
    // wait for a proper response
    nsTime tStart = nsTime::Now();
    while (nsTime::Now() - tStart < timeout && !bServerFound)
    {
      network->Send('FSRV', 'RUTR');

      nsThreadUtils::Sleep(nsTime::MakeFromMilliseconds(100));

      network->UpdateRemoteInterface();
      network->ExecuteAllMessageHandlers();
    }
  }

  network->ShutdownConnection();

  if (!bServerFound)
    return NS_FAILURE;

  m_sServerConnectionAddress = szAddress;

  // always store the IP that was successful in the user directory
  SaveCurrentConnectionInfoToDisk().IgnoreResult();
  return NS_SUCCESS;
}

nsResult nsFileserveClient::WaitForServerInfo(nsTime timeout /*= nsTime::MakeFromSeconds(60.0 * 5)*/)
{
  NS_LOCK(m_Mutex);
  if (!s_bEnableFileserve)
    return NS_FAILURE;

  nsUInt16 uiPort = 1042;
  nsHybridArray<nsStringBuilder, 4> sServerIPs;

  {
    nsUniquePtr<nsRemoteInterfaceEnet> network = nsRemoteInterfaceEnet::Make(); /// \todo Abstract this somehow ?
    network->SetMessageHandler('FSRV', [&sServerIPs, &uiPort](nsRemoteMessage& ref_msg)

      {
        switch (ref_msg.GetMessageID())
        {
          case 'MYIP':
            ref_msg.GetReader() >> uiPort;

            nsUInt8 uiCount = 0;
            ref_msg.GetReader() >> uiCount;

            sServerIPs.SetCount(uiCount);
            for (nsUInt32 i = 0; i < uiCount; ++i)
            {
              ref_msg.GetReader() >> sServerIPs[i];
            }

            break;
        } });

    NS_SUCCEED_OR_RETURN(network->StartServer('NSIP', "2042", false));

    nsTime tStart = nsTime::Now();
    while (nsTime::Now() - tStart < timeout && sServerIPs.IsEmpty())
    {
      nsThreadUtils::Sleep(nsTime::MakeFromMilliseconds(1));

      network->UpdateRemoteInterface();
      network->ExecuteAllMessageHandlers();
    }

    network->ShutdownConnection();
  }

  if (sServerIPs.IsEmpty())
    return NS_FAILURE;

  // network connections are unreliable and surprisingly slow sometimes
  // we just got an IP from a server, so we know it's there and we should be able to connect to it
  // still this often fails the first few times
  // so we try this several times and waste some time in between and hope that at some point the connection succeeds
  for (nsUInt32 i = 0; i < 8; ++i)
  {
    nsStringBuilder sAddress;
    for (auto& ip : sServerIPs)
    {
      sAddress.SetFormat("{0}:{1}", ip, uiPort);

      nsThreadUtils::Sleep(nsTime::MakeFromMilliseconds(500));

      if (TryConnectWithFileserver(sAddress, nsTime::MakeFromSeconds(3)).Succeeded())
        return NS_SUCCESS;
    }

    nsThreadUtils::Sleep(nsTime::MakeFromMilliseconds(1000));
  }

  return NS_FAILURE;
}

nsResult nsFileserveClient::SaveCurrentConnectionInfoToDisk() const
{
  NS_LOCK(m_Mutex);
  nsStringBuilder sFile = nsOSFile::GetUserDataFolder("nsFileserve.txt");
  nsOSFile file;
  NS_SUCCEED_OR_RETURN(file.Open(sFile, nsFileOpenMode::Write));

  NS_SUCCEED_OR_RETURN(file.Write(m_sServerConnectionAddress.GetData(), m_sServerConnectionAddress.GetElementCount()));
  file.Close();

  return NS_SUCCESS;
}

NS_ON_GLOBAL_EVENT(GameApp_UpdatePlugins)
{
  if (nsFileserveClient::GetSingleton())
  {
    nsFileserveClient::GetSingleton()->UpdateClient();
  }
}



NS_STATICLINK_FILE(FileservePlugin, FileservePlugin_Client_FileserveClient);
