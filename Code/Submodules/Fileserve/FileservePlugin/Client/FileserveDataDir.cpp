#include <FileservePlugin/FileservePluginPCH.h>

#include <FileservePlugin/Client/FileserveDataDir.h>
#include <Foundation/Algorithm/HashingUtils.h>
#include <Foundation/Communication/RemoteInterfaceEnet.h>
#include <Foundation/Logging/Log.h>

void nsDataDirectory::FileserveType::ReloadExternalConfigs()
{
  NS_LOCK(m_RedirectionMutex);
  m_FileRedirection.Clear();

  if (!s_sRedirectionFile.IsEmpty())
  {
    nsFileserveClient::GetSingleton()->DownloadFile(m_uiDataDirID, s_sRedirectionFile, true, nullptr).IgnoreResult();
  }

  FolderType::ReloadExternalConfigs();
}

nsDataDirectoryReader* nsDataDirectory::FileserveType::OpenFileToRead(nsStringView sFile, nsFileShareMode::Enum FileShareMode, bool bSpecificallyThisDataDir)
{
  // fileserve cannot handle absolute paths, which is actually already ruled out at creation time, so this is just an optimization
  if (nsPathUtils::IsAbsolutePath(sFile))
    return nullptr;

  nsStringBuilder sRedirected;
  if (ResolveAssetRedirection(sFile, sRedirected))
    bSpecificallyThisDataDir = true; // If this data dir can resolve the guid, only this should load it as well.

  // we know that the server cannot resolve asset GUIDs, so don't even ask
  if (nsConversionUtils::IsStringUuid(sRedirected))
    return nullptr;

  nsStringBuilder sFullPath;
  if (nsFileserveClient::GetSingleton()->DownloadFile(m_uiDataDirID, sRedirected, bSpecificallyThisDataDir, &sFullPath).Failed())
    return nullptr;

  // It's fine to use the base class here as it will resurface in CreateFolderReader which gives us control of the important part.
  return FolderType::OpenFileToRead(sFullPath, FileShareMode, bSpecificallyThisDataDir);
}

nsDataDirectoryWriter* nsDataDirectory::FileserveType::OpenFileToWrite(nsStringView sFile, nsFileShareMode::Enum FileShareMode)
{
  // fileserve cannot handle absolute paths, which is actually already ruled out at creation time, so this is just an optimization
  if (nsPathUtils::IsAbsolutePath(sFile))
    return nullptr;

  return FolderType::OpenFileToWrite(sFile, FileShareMode);
}

nsResult nsDataDirectory::FileserveType::InternalInitializeDataDirectory(nsStringView sDirectory)
{
  nsStringBuilder sDataDir = sDirectory;
  sDataDir.MakeCleanPath();

  nsStringBuilder sCacheFolder, sCacheMetaFolder;
  nsFileserveClient::GetSingleton()->GetFullDataDirCachePath(sDataDir, sCacheFolder, sCacheMetaFolder);
  m_sRedirectedDataDirPath = sCacheFolder;
  m_sFileserveCacheMetaFolder = sCacheMetaFolder;

  ReloadExternalConfigs();
  return NS_SUCCESS;
}

void nsDataDirectory::FileserveType::RemoveDataDirectory()
{
  if (nsFileserveClient::GetSingleton())
  {
    nsFileserveClient::GetSingleton()->UnmountDataDirectory(m_uiDataDirID);
  }

  FolderType::RemoveDataDirectory();
}

void nsDataDirectory::FileserveType::DeleteFile(nsStringView sFile)
{
  if (nsFileserveClient::GetSingleton())
  {
    nsFileserveClient::GetSingleton()->DeleteFile(m_uiDataDirID, sFile);
  }

  FolderType::DeleteFile(sFile);
}

nsDataDirectory::FolderReader* nsDataDirectory::FileserveType::CreateFolderReader() const
{
  return NS_DEFAULT_NEW(FileserveDataDirectoryReader, 0);
}

nsDataDirectory::FolderWriter* nsDataDirectory::FileserveType::CreateFolderWriter() const
{
  return NS_DEFAULT_NEW(FileserveDataDirectoryWriter);
}

nsResult nsDataDirectory::FileserveType::GetFileStats(nsStringView sFileOrFolder, bool bOneSpecificDataDir, nsFileStats& out_Stats)
{
  nsStringBuilder sRedirected;
  if (ResolveAssetRedirection(sFileOrFolder, sRedirected))
    bOneSpecificDataDir = true; // If this data dir can resolve the guid, only this should load it as well.

  // we know that the server cannot resolve asset GUIDs, so don't even ask
  if (nsConversionUtils::IsStringUuid(sRedirected))
    return NS_FAILURE;

  nsStringBuilder sFullPath;
  NS_SUCCEED_OR_RETURN(nsFileserveClient::GetSingleton()->DownloadFile(m_uiDataDirID, sRedirected, bOneSpecificDataDir, &sFullPath));
  return nsOSFile::GetFileStats(sFullPath, out_Stats);
}

bool nsDataDirectory::FileserveType::ExistsFile(nsStringView sFile, bool bOneSpecificDataDir)
{
  nsStringBuilder sRedirected;
  if (ResolveAssetRedirection(sFile, sRedirected))
    bOneSpecificDataDir = true; // If this data dir can resolve the guid, only this should load it as well.

  // we know that the server cannot resolve asset GUIDs, so don't even ask
  if (nsConversionUtils::IsStringUuid(sRedirected))
    return false;

  return nsFileserveClient::GetSingleton()->DownloadFile(m_uiDataDirID, sRedirected, bOneSpecificDataDir, nullptr).Succeeded();
}

nsDataDirectoryType* nsDataDirectory::FileserveType::Factory(nsStringView sDataDirectory, nsStringView sGroup, nsStringView sRootName, nsFileSystem::DataDirUsage usage)
{
  if (!nsFileserveClient::s_bEnableFileserve || nsFileserveClient::GetSingleton() == nullptr)
    return nullptr; // this would only happen if the functionality is switched off, but not before the factory was added

  // ignore the empty data dir, which handles absolute paths, as we cannot translate these paths to the fileserve host OS
  if (sDataDirectory.IsEmpty())
    return nullptr;

  // Fileserve can only translate paths on the server that start with a 'Special Directory' (e.g. ">sdk/" or ">project/")
  // ignore everything else
  if (!sDataDirectory.StartsWith(">"))
    return nullptr;

  if (nsFileserveClient::GetSingleton()->EnsureConnected().Failed())
    return nullptr;

  nsDataDirectory::FileserveType* pDataDir = NS_DEFAULT_NEW(nsDataDirectory::FileserveType);
  pDataDir->m_uiDataDirID = nsFileserveClient::GetSingleton()->MountDataDirectory(sDataDirectory, sRootName);

  if (pDataDir->m_uiDataDirID < 0xffff && pDataDir->InitializeDataDirectory(sDataDirectory) == NS_SUCCESS)
    return pDataDir;

  NS_DEFAULT_DELETE(pDataDir);
  return nullptr;
}

nsDataDirectory::FileserveDataDirectoryReader::FileserveDataDirectoryReader(nsInt32 iDataDirUserData)
  : FolderReader(iDataDirUserData)
{
}

nsResult nsDataDirectory::FileserveDataDirectoryReader::InternalOpen(nsFileShareMode::Enum FileShareMode)
{
  return m_File.Open(GetFilePath().GetData(), nsFileOpenMode::Read, FileShareMode);
}

void nsDataDirectory::FileserveDataDirectoryWriter::InternalClose()
{
  FolderWriter::InternalClose();

  static_cast<FileserveType*>(GetDataDirectory())->FinishedWriting(this);
}

void nsDataDirectory::FileserveType::FinishedWriting(FolderWriter* pWriter)
{
  if (nsFileserveClient::GetSingleton() == nullptr)
    return;

  nsStringBuilder sAbsPath = pWriter->GetDataDirectory()->GetRedirectedDataDirectoryPath();
  sAbsPath.AppendPath(pWriter->GetFilePath());

  nsOSFile file;
  if (file.Open(sAbsPath, nsFileOpenMode::Read).Failed())
  {
    nsLog::Error("Could not read file for upload: '{0}'", sAbsPath);
    return;
  }

  nsDynamicArray<nsUInt8> content;
  file.ReadAll(content);
  file.Close();

  nsFileserveClient::GetSingleton()->UploadFile(m_uiDataDirID, pWriter->GetFilePath(), content);
}



NS_STATICLINK_FILE(FileservePlugin, FileservePlugin_Client_FileserveDataDir);
