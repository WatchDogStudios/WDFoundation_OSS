#pragma once

#include <FileservePlugin/Client/FileserveClient.h>
#include <Foundation/Communication/RemoteInterface.h>
#include <Foundation/Containers/HybridArray.h>
#include <Foundation/Containers/Map.h>
#include <Foundation/IO/FileSystem/DataDirTypeFolder.h>
#include <Foundation/IO/FileSystem/Implementation/DataDirType.h>
#include <Foundation/IO/OSFile.h>
#include <Foundation/Types/UniquePtr.h>

namespace nsDataDirectory
{
  class FileserveDataDirectoryReader : public FolderReader
  {
  public:
    FileserveDataDirectoryReader(nsInt32 iDataDirUserData);

  protected:
    virtual nsResult InternalOpen(nsFileShareMode::Enum FileShareMode) override;
  };

  class FileserveDataDirectoryWriter : public FolderWriter
  {
  protected:
    virtual void InternalClose() override;
  };

  /// \brief A data directory type to handle access to files that are served from a network host.
  class NS_FILESERVEPLUGIN_DLL FileserveType : public FolderType
  {
  public:
    /// \brief The factory that can be registered at nsFileSystem to create data directories of this type.
    static nsDataDirectoryType* Factory(nsStringView sDataDirectory, nsStringView sGroup, nsStringView sRootName, nsFileSystem::DataDirUsage usage);

    /// \brief [internal] Makes sure the redirection config files are up to date and then reloads them.
    virtual void ReloadExternalConfigs() override;

    /// \brief [internal] Called by FileserveDataDirectoryWriter when it is finished to upload the written file to the server
    void FinishedWriting(FolderWriter* pWriter);

  protected:
    virtual nsDataDirectoryReader* OpenFileToRead(nsStringView sFile, nsFileShareMode::Enum FileShareMode, bool bSpecificallyThisDataDir) override;
    virtual nsDataDirectoryWriter* OpenFileToWrite(nsStringView sFile, nsFileShareMode::Enum FileShareMode) override;
    virtual nsResult InternalInitializeDataDirectory(nsStringView sDirectory) override;
    virtual void RemoveDataDirectory() override;
    virtual void DeleteFile(nsStringView sFile) override;
    virtual bool ExistsFile(nsStringView sFile, bool bOneSpecificDataDir) override;
    /// \brief Limitation: Fileserve does not handle folders, only files. If someone stats a folder, this will fail.
    virtual nsResult GetFileStats(nsStringView sFileOrFolder, bool bOneSpecificDataDir, nsFileStats& out_Stats) override;
    virtual FolderReader* CreateFolderReader() const override;
    virtual FolderWriter* CreateFolderWriter() const override;

    nsUInt16 m_uiDataDirID = 0xffff;
    nsString128 m_sFileserveCacheMetaFolder;
  };
} // namespace nsDataDirectory
