#include <FileservePlugin/FileservePluginPCH.h>

#include <FileservePlugin/Client/FileserveClient.h>
#include <FileservePlugin/Fileserver/Fileserver.h>
#include <Foundation/Algorithm/HashingUtils.h>
#include <Foundation/Communication/RemoteInterfaceEnet.h>
#include <Foundation/IO/FileSystem/FileReader.h>
#include <Foundation/Utilities/CommandLineUtils.h>

NS_IMPLEMENT_SINGLETON(nsFileserver);

nsFileserver::nsFileserver()
  : m_SingletonRegistrar(this)
{
  // once a server exists, the client should stay inactive
  nsFileserveClient::DisabledFileserveClient();

  // check whether the fileserve port was reconfigured through the command line
  m_uiPort = static_cast<nsUInt16>(nsCommandLineUtils::GetGlobalInstance()->GetIntOption("-fs_port", m_uiPort));
}

void nsFileserver::StartServer()
{
  if (m_pNetwork)
    return;

  nsStringBuilder tmp;

  m_pNetwork = nsRemoteInterfaceEnet::Make();
  m_pNetwork->StartServer('NSFS', nsConversionUtils::ToString(m_uiPort, tmp), false).IgnoreResult();
  m_pNetwork->SetMessageHandler('FSRV', nsMakeDelegate(&nsFileserver::NetworkMsgHandler, this));
  m_pNetwork->m_RemoteEvents.AddEventHandler(nsMakeDelegate(&nsFileserver::NetworkEventHandler, this));

  nsFileserverEvent e;
  e.m_Type = nsFileserverEvent::Type::ServerStarted;
  m_Events.Broadcast(e);
}

void nsFileserver::StopServer()
{
  if (!m_pNetwork)
    return;

  m_pNetwork->ShutdownConnection();
  m_pNetwork.Clear();

  nsFileserverEvent e;
  e.m_Type = nsFileserverEvent::Type::ServerStopped;
  m_Events.Broadcast(e);
}

bool nsFileserver::UpdateServer()
{
  if (!m_pNetwork)
    return false;

  m_pNetwork->UpdateRemoteInterface();
  return m_pNetwork->ExecuteAllMessageHandlers() > 0;
}

bool nsFileserver::IsServerRunning() const
{
  return m_pNetwork != nullptr;
}

void nsFileserver::SetPort(nsUInt16 uiPort)
{
  NS_ASSERT_DEV(m_pNetwork == nullptr, "The port cannot be changed after the server was started");
  m_uiPort = uiPort;
}


void nsFileserver::BroadcastReloadResourcesCommand()
{
  if (!IsServerRunning())
    return;

  m_pNetwork->Send('FSRV', 'RLDR');
}

void nsFileserver::NetworkMsgHandler(nsRemoteMessage& msg)
{
  auto& client = DetermineClient(msg);

  if (msg.GetMessageID() == 'HELO')
    return;

  if (msg.GetMessageID() == 'RUTR')
  {
    // 'are you there' is used to check whether a certain address is a proper Fileserver
    m_pNetwork->Send('FSRV', ' YES');

    nsFileserverEvent e;
    e.m_Type = nsFileserverEvent::Type::AreYouThereRequest;
    m_Events.Broadcast(e);
    return;
  }

  if (msg.GetMessageID() == 'READ')
  {
    HandleFileRequest(client, msg);
    return;
  }

  if (msg.GetMessageID() == 'UPLH')
  {
    HandleUploadFileHeader(client, msg);
    return;
  }

  if (msg.GetMessageID() == 'UPLD')
  {
    HandleUploadFileTransfer(client, msg);
    return;
  }

  if (msg.GetMessageID() == 'UPLF')
  {
    HandleUploadFileFinished(client, msg);
    return;
  }

  if (msg.GetMessageID() == 'DELF')
  {
    HandleDeleteFileRequest(client, msg);
    return;
  }

  if (msg.GetMessageID() == ' MNT')
  {
    HandleMountRequest(client, msg);
    return;
  }

  if (msg.GetMessageID() == 'UMNT')
  {
    HandleUnmountRequest(client, msg);
    return;
  }

  nsLog::Error("Unknown FSRV message: '{0}' - {1} bytes", msg.GetMessageID(), msg.GetMessageData().GetCount());
}


void nsFileserver::NetworkEventHandler(const nsRemoteEvent& e)
{
  switch (e.m_Type)
  {
    case nsRemoteEvent::DisconnectedFromClient:
    {
      if (m_Clients.Contains(e.m_uiOtherAppID))
      {
        nsFileserverEvent se;
        se.m_Type = nsFileserverEvent::Type::ClientDisconnected;
        se.m_uiClientID = e.m_uiOtherAppID;

        m_Events.Broadcast(se);

        m_Clients[e.m_uiOtherAppID].m_bLostConnection = true;
      }
    }
    break;

    default:
      break;
  }
}

nsFileserveClientContext& nsFileserver::DetermineClient(nsRemoteMessage& msg)
{
  nsFileserveClientContext& client = m_Clients[msg.GetApplicationID()];

  if (client.m_uiApplicationID != msg.GetApplicationID())
  {
    client.m_uiApplicationID = msg.GetApplicationID();

    nsFileserverEvent e;
    e.m_Type = nsFileserverEvent::Type::ClientConnected;
    e.m_uiClientID = client.m_uiApplicationID;
    m_Events.Broadcast(e);
  }
  else if (client.m_bLostConnection)
  {
    client.m_bLostConnection = false;

    nsFileserverEvent e;
    e.m_Type = nsFileserverEvent::Type::ClientReconnected;
    e.m_uiClientID = client.m_uiApplicationID;
    m_Events.Broadcast(e);
  }

  return client;
}

void nsFileserver::HandleMountRequest(nsFileserveClientContext& client, nsRemoteMessage& msg)
{
  nsStringBuilder sDataDir, sRootName, sMountPoint, sRedir;
  nsUInt16 uiDataDirID = 0xffff;

  msg.GetReader() >> sDataDir;
  msg.GetReader() >> sRootName;
  msg.GetReader() >> sMountPoint;
  msg.GetReader() >> uiDataDirID;

  NS_ASSERT_DEV(uiDataDirID >= client.m_MountedDataDirs.GetCount(), "Data dir ID should be larger than previous IDs");

  client.m_MountedDataDirs.SetCount(nsMath::Max<nsUInt32>(uiDataDirID + 1, client.m_MountedDataDirs.GetCount()));
  auto& dir = client.m_MountedDataDirs[uiDataDirID];
  dir.m_sPathOnClient = sDataDir;
  dir.m_sRootName = sRootName;
  dir.m_sMountPoint = sMountPoint;

  nsFileserverEvent e;

  if (nsFileSystem::ResolveSpecialDirectory(sDataDir, sRedir).Succeeded())
  {
    dir.m_bMounted = true;
    dir.m_sPathOnServer = sRedir;
    e.m_Type = nsFileserverEvent::Type::MountDataDir;
  }
  else
  {
    dir.m_bMounted = false;
    e.m_Type = nsFileserverEvent::Type::MountDataDirFailed;
  }

  e.m_uiClientID = client.m_uiApplicationID;
  e.m_szName = sRootName;
  e.m_szPath = sDataDir;
  e.m_szRedirectedPath = sRedir;
  m_Events.Broadcast(e);
}


void nsFileserver::HandleUnmountRequest(nsFileserveClientContext& client, nsRemoteMessage& msg)
{
  nsUInt16 uiDataDirID = 0xffff;
  msg.GetReader() >> uiDataDirID;

  NS_ASSERT_DEV(uiDataDirID < client.m_MountedDataDirs.GetCount(), "Invalid data dir ID to unmount");

  auto& dir = client.m_MountedDataDirs[uiDataDirID];
  dir.m_bMounted = false;

  nsFileserverEvent e;
  e.m_Type = nsFileserverEvent::Type::UnmountDataDir;
  e.m_uiClientID = client.m_uiApplicationID;
  e.m_szPath = dir.m_sPathOnClient;
  e.m_szName = dir.m_sRootName;
  m_Events.Broadcast(e);
}

void nsFileserver::HandleFileRequest(nsFileserveClientContext& client, nsRemoteMessage& msg)
{
  nsUInt16 uiDataDirID = 0;
  bool bForceThisDataDir = false;

  msg.GetReader() >> uiDataDirID;
  msg.GetReader() >> bForceThisDataDir;

  nsStringBuilder sRequestedFile;
  msg.GetReader() >> sRequestedFile;

  nsUuid downloadGuid;
  msg.GetReader() >> downloadGuid;

  nsFileserveClientContext::FileStatus status;
  msg.GetReader() >> status.m_iTimestamp;
  msg.GetReader() >> status.m_uiHash;

  nsFileserverEvent e;
  e.m_uiClientID = client.m_uiApplicationID;
  e.m_szPath = sRequestedFile;
  e.m_uiSentTotal = 0;

  const nsFileserveFileState filestate = client.GetFileStatus(uiDataDirID, sRequestedFile, status, m_SendToClient, bForceThisDataDir);

  {
    e.m_Type = nsFileserverEvent::Type::FileDownloadRequest;
    e.m_uiSizeTotal = m_SendToClient.GetCount();
    e.m_FileState = filestate;
    m_Events.Broadcast(e);
  }

  if (filestate == nsFileserveFileState::Different)
  {
    nsUInt32 uiNextByte = 0;
    const nsUInt32 uiFileSize = m_SendToClient.GetCount();

    // send the file over in multiple packages of 1KB each
    // send at least one package, even for empty files
    do
    {
      const nsUInt16 uiChunkSize = (nsUInt16)nsMath::Min<nsUInt32>(1024, m_SendToClient.GetCount() - uiNextByte);

      nsRemoteMessage ret;
      ret.GetWriter() << downloadGuid;
      ret.GetWriter() << uiChunkSize;
      ret.GetWriter() << uiFileSize;

      if (!m_SendToClient.IsEmpty())
        ret.GetWriter().WriteBytes(&m_SendToClient[uiNextByte], uiChunkSize).IgnoreResult();

      ret.SetMessageID('FSRV', 'DWNL');
      m_pNetwork->Send(nsRemoteTransmitMode::Reliable, ret);

      uiNextByte += uiChunkSize;

      // reuse previous values
      {
        e.m_Type = nsFileserverEvent::Type::FileDownloading;
        e.m_uiSentTotal = uiNextByte;
        m_Events.Broadcast(e);
      }
    } while (uiNextByte < m_SendToClient.GetCount());
  }

  // final answer to client
  {
    nsRemoteMessage ret('FSRV', 'DWNF');
    ret.GetWriter() << downloadGuid;
    ret.GetWriter() << (nsInt8)filestate;
    ret.GetWriter() << status.m_iTimestamp;
    ret.GetWriter() << status.m_uiHash;
    ret.GetWriter() << uiDataDirID;

    m_pNetwork->Send(nsRemoteTransmitMode::Reliable, ret);
  }

  // reuse previous values
  {
    e.m_Type = nsFileserverEvent::Type::FileDownloadFinished;
    m_Events.Broadcast(e);
  }
}

void nsFileserver::HandleDeleteFileRequest(nsFileserveClientContext& client, nsRemoteMessage& msg)
{
  nsUInt16 uiDataDirID = 0xffff;
  msg.GetReader() >> uiDataDirID;

  nsStringBuilder sFile;
  msg.GetReader() >> sFile;

  NS_ASSERT_DEV(uiDataDirID < client.m_MountedDataDirs.GetCount(), "Invalid data dir ID to unmount");

  nsFileserverEvent e;
  e.m_Type = nsFileserverEvent::Type::FileDeleteRequest;
  e.m_uiClientID = client.m_uiApplicationID;
  e.m_szPath = sFile;
  m_Events.Broadcast(e);

  const auto& dd = client.m_MountedDataDirs[uiDataDirID];

  nsStringBuilder sAbsPath;
  sAbsPath = dd.m_sPathOnServer;
  sAbsPath.AppendPath(sFile);

  nsOSFile::DeleteFile(sAbsPath).IgnoreResult();
}

void nsFileserver::HandleUploadFileHeader(nsFileserveClientContext& client, nsRemoteMessage& msg)
{
  nsUInt16 uiDataDirID = 0;

  msg.GetReader() >> m_FileUploadGuid;
  msg.GetReader() >> m_uiFileUploadSize;
  msg.GetReader() >> uiDataDirID;
  msg.GetReader() >> m_sCurFileUpload;

  m_SentFromClient.Clear();
  m_SentFromClient.Reserve(m_uiFileUploadSize);

  nsFileserverEvent e;
  e.m_Type = nsFileserverEvent::Type::FileUploadRequest;
  e.m_uiClientID = client.m_uiApplicationID;
  e.m_szPath = m_sCurFileUpload;
  e.m_uiSentTotal = 0;
  e.m_uiSizeTotal = m_uiFileUploadSize;

  m_Events.Broadcast(e);
}

void nsFileserver::HandleUploadFileTransfer(nsFileserveClientContext& client, nsRemoteMessage& msg)
{
  nsUuid transferGuid;
  msg.GetReader() >> transferGuid;

  if (transferGuid != m_FileUploadGuid)
    return;

  nsUInt16 uiChunkSize = 0;
  msg.GetReader() >> uiChunkSize;

  const nsUInt32 uiStartPos = m_SentFromClient.GetCount();
  m_SentFromClient.SetCountUninitialized(uiStartPos + uiChunkSize);
  msg.GetReader().ReadBytes(&m_SentFromClient[uiStartPos], uiChunkSize);

  nsFileserverEvent e;
  e.m_Type = nsFileserverEvent::Type::FileUploading;
  e.m_uiClientID = client.m_uiApplicationID;
  e.m_szPath = m_sCurFileUpload;
  e.m_uiSentTotal = m_SentFromClient.GetCount();
  e.m_uiSizeTotal = m_uiFileUploadSize;

  m_Events.Broadcast(e);
}

void nsFileserver::HandleUploadFileFinished(nsFileserveClientContext& client, nsRemoteMessage& msg)
{
  nsUuid transferGuid;
  msg.GetReader() >> transferGuid;

  if (transferGuid != m_FileUploadGuid)
    return;

  nsUInt16 uiDataDirID = 0;
  msg.GetReader() >> uiDataDirID;

  nsStringBuilder sFile;
  msg.GetReader() >> sFile;

  nsStringBuilder sOutputFile;
  sOutputFile = client.m_MountedDataDirs[uiDataDirID].m_sPathOnServer;
  sOutputFile.AppendPath(sFile);

  {
    nsOSFile file;
    if (file.Open(sOutputFile, nsFileOpenMode::Write).Failed())
    {
      nsLog::Error("Could not write uploaded file to '{0}'", sOutputFile);
      return;
    }

    if (!m_SentFromClient.IsEmpty())
    {
      file.Write(m_SentFromClient.GetData(), m_SentFromClient.GetCount()).IgnoreResult();
    }
  }

  nsFileserverEvent e;
  e.m_Type = nsFileserverEvent::Type::FileUploadFinished;
  e.m_uiClientID = client.m_uiApplicationID;
  e.m_szPath = sFile;
  e.m_uiSentTotal = m_SentFromClient.GetCount();
  e.m_uiSizeTotal = m_SentFromClient.GetCount();

  m_Events.Broadcast(e);

  // send a response when all data has been transmitted
  // this ensures the client side updates the network until all data has been fully transmitted
  m_pNetwork->Send('FSRV', 'UACK');
}


nsResult nsFileserver::SendConnectionInfo(const char* szClientAddress, nsUInt16 uiMyPort, const nsArrayPtr<nsStringBuilder>& myIPs, nsTime timeout)
{
  nsStringBuilder sAddress = szClientAddress;
  sAddress.Append(":2042"); // hard-coded port

  nsUniquePtr<nsRemoteInterfaceEnet> network = nsRemoteInterfaceEnet::Make();
  NS_SUCCEED_OR_RETURN(network->ConnectToServer('NSIP', sAddress, false));

  if (network->WaitForConnectionToServer(timeout).Failed())
  {
    network->ShutdownConnection();
    return NS_FAILURE;
  }

  const nsUInt8 uiCount = static_cast<nsUInt8>(myIPs.GetCount());

  nsRemoteMessage msg('FSRV', 'MYIP');
  msg.GetWriter() << uiMyPort;
  msg.GetWriter() << uiCount;

  for (const auto& info : myIPs)
  {
    msg.GetWriter() << info;
  }

  network->Send(nsRemoteTransmitMode::Reliable, msg);

  // make sure the message is out, before we shut down
  for (nsUInt32 i = 0; i < 10; ++i)
  {
    network->UpdateRemoteInterface();
    nsThreadUtils::Sleep(nsTime::MakeFromMilliseconds(1));
  }

  network->ShutdownConnection();
  return NS_SUCCESS;
}



NS_STATICLINK_FILE(FileservePlugin, FileservePlugin_Fileserver_Fileserver);
