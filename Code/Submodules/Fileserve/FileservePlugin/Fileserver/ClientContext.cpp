#include <FileservePlugin/FileservePluginPCH.h>

#include <FileservePlugin/Fileserver/ClientContext.h>
#include <Foundation/IO/FileSystem/FileReader.h>

nsFileserveFileState nsFileserveClientContext::GetFileStatus(nsUInt16& inout_uiDataDirID, const char* szRequestedFile, FileStatus& inout_status,
  nsDynamicArray<nsUInt8>& out_fileContent, bool bForceThisDataDir) const
{
  for (nsUInt16 i = static_cast<nsUInt16>(m_MountedDataDirs.GetCount()); i > 0; --i)
  {
    const nsUInt16 uiDataDirID = i - 1;

    // when !bForceThisDataDir search for the best match
    // otherwise lookup this exact data dir
    if (bForceThisDataDir && uiDataDirID != inout_uiDataDirID)
      continue;

    const auto& dd = m_MountedDataDirs[uiDataDirID];

    if (!dd.m_bMounted)
      continue;

    nsStringBuilder sAbsPath;
    sAbsPath = dd.m_sPathOnServer;
    sAbsPath.AppendPath(szRequestedFile);

    nsFileStats stat;
    if (nsOSFile::GetFileStats(sAbsPath, stat).Failed())
      continue;

    inout_status.m_uiFileSize = stat.m_uiFileSize;

    const nsInt64 iNewTimestamp = stat.m_LastModificationTime.GetInt64(nsSIUnitOfTime::Microsecond);

    if (inout_status.m_iTimestamp == iNewTimestamp && inout_uiDataDirID == uiDataDirID)
      return nsFileserveFileState::SameTimestamp;

    inout_status.m_iTimestamp = iNewTimestamp;

    // read the entire file
    {
      nsFileReader file;
      if (file.Open(sAbsPath).Failed())
        continue;

      nsUInt64 uiNewHash = 1;
      out_fileContent.SetCountUninitialized((nsUInt32)inout_status.m_uiFileSize);

      if (!out_fileContent.IsEmpty())
      {
        file.ReadBytes(out_fileContent.GetData(), out_fileContent.GetCount());
        uiNewHash = nsHashingUtils::xxHash64(out_fileContent.GetData(), (size_t)out_fileContent.GetCount(), uiNewHash);

        // if the file is empty, the hash will be zero, which could lead to an incorrect assumption that the hash is the same
        // instead always transfer the empty file, so that it properly exists on the client
        if (inout_status.m_uiHash == uiNewHash && inout_uiDataDirID == uiDataDirID)
          return nsFileserveFileState::SameHash;
      }

      inout_status.m_uiHash = uiNewHash;
    }

    inout_uiDataDirID = uiDataDirID;
    return nsFileserveFileState::Different;
  }

  inout_status.m_iTimestamp = 0;
  inout_status.m_uiFileSize = 0;
  inout_status.m_uiHash = 0;

  // the client doesn't have the file either
  // this is an optimization to prevent redundant file deletions on the client
  if (inout_status.m_iTimestamp == 0 && inout_status.m_uiHash == 0)
    return nsFileserveFileState::NonExistantEither;

  return nsFileserveFileState::NonExistant;
}



NS_STATICLINK_FILE(FileservePlugin, FileservePlugin_Fileserver_ClientContext);
