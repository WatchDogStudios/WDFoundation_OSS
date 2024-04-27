#pragma once

#include <FileservePlugin/FileservePluginDLL.h>
#include <Foundation/Containers/HybridArray.h>
#include <Foundation/Strings/String.h>

enum class nsFileserveFileState
{
  None = 0,
  NonExistant = 1,
  NonExistantEither = 2,
  SameTimestamp = 3,
  SameHash = 4,
  Different = 5,
};

class NS_FILESERVEPLUGIN_DLL nsFileserveClientContext
{
public:
  struct DataDir
  {
    nsString m_sRootName;
    nsString m_sPathOnClient;
    nsString m_sPathOnServer;
    nsString m_sMountPoint;
    bool m_bMounted = false;
  };

  struct FileStatus
  {
    nsInt64 m_iTimestamp = -1;
    nsUInt64 m_uiHash = 0;
    nsUInt64 m_uiFileSize = 0;
  };

  nsFileserveFileState GetFileStatus(nsUInt16& inout_uiDataDirID, const char* szRequestedFile, FileStatus& inout_status,
    nsDynamicArray<nsUInt8>& out_fileContent, bool bForceThisDataDir) const;

  bool m_bLostConnection = false;
  nsUInt32 m_uiApplicationID = 0;
  nsHybridArray<DataDir, 8> m_MountedDataDirs;
};
