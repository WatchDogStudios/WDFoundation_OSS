/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Configuration/Startup.h>
#include <Foundation/IO/Archive/ArchiveUtils.h>
#include <Foundation/IO/Archive/DataDirTypeArchive.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/IO/OSFile.h>
#include <Foundation/Logging/Log.h>

// clang-format off
NS_BEGIN_SUBSYSTEM_DECLARATION(Foundation, ArchiveDataDirectory)

  BEGIN_SUBSYSTEM_DEPENDENCIES
    "FileSystem", "FolderDataDirectory"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
  nsFileSystem::RegisterDataDirectoryFactory(nsDataDirectory::ArchiveType::Factory);
  }

NS_END_SUBSYSTEM_DECLARATION;
// clang-format on

nsDataDirectory::ArchiveType::ArchiveType() = default;
nsDataDirectory::ArchiveType::~ArchiveType() = default;

nsDataDirectoryType* nsDataDirectory::ArchiveType::Factory(nsStringView sDataDirectory, nsStringView sGroup, nsStringView sRootName, nsFileSystem::DataDirUsage usage)
{
  ArchiveType* pDataDir = NS_DEFAULT_NEW(ArchiveType);

  if (pDataDir->InitializeDataDirectory(sDataDirectory) == NS_SUCCESS)
    return pDataDir;

  NS_DEFAULT_DELETE(pDataDir);
  return nullptr;
}

nsDataDirectoryReader* nsDataDirectory::ArchiveType::OpenFileToRead(nsStringView sFile, nsFileShareMode::Enum FileShareMode, bool bSpecificallyThisDataDir)
{
  const nsArchiveTOC& toc = m_ArchiveReader.GetArchiveTOC();
  nsStringBuilder sArchivePath = m_sArchiveSubFolder;
  sArchivePath.AppendPath(sFile);

  const nsUInt32 uiEntryIndex = toc.FindEntry(sArchivePath);

  if (uiEntryIndex == nsInvalidIndex)
    return nullptr;

  const nsArchiveEntry* pEntry = &toc.m_Entries[uiEntryIndex];

  ArchiveReaderUncompressed* pReader = nullptr;

  {
    NS_LOCK(m_ReaderMutex);

    switch (pEntry->m_CompressionMode)
    {
      case nsArchiveCompressionMode::Uncompressed:
      {
        if (!m_FreeReadersUncompressed.IsEmpty())
        {
          pReader = m_FreeReadersUncompressed.PeekBack();
          m_FreeReadersUncompressed.PopBack();
        }
        else
        {
          m_ReadersUncompressed.PushBack(NS_DEFAULT_NEW(ArchiveReaderUncompressed, 0));
          pReader = m_ReadersUncompressed.PeekBack().Borrow();
        }
        break;
      }

#ifdef BUILDSYSTEM_ENABLE_ZSTD_SUPPORT
      case nsArchiveCompressionMode::Compressed_zstd:
      {
        if (!m_FreeReadersZstd.IsEmpty())
        {
          pReader = m_FreeReadersZstd.PeekBack();
          m_FreeReadersZstd.PopBack();
        }
        else
        {
          m_ReadersZstd.PushBack(NS_DEFAULT_NEW(ArchiveReaderZstd, 1));
          pReader = m_ReadersZstd.PeekBack().Borrow();
        }
        break;
      }
#endif

      default:
        NS_REPORT_FAILURE("Compression mode {} is unknown (or not compiled in)", (nsUInt8)pEntry->m_CompressionMode);
        return nullptr;
    }
  }

  pReader->m_uiUncompressedSize = pEntry->m_uiUncompressedDataSize;
  pReader->m_uiCompressedSize = pEntry->m_uiStoredDataSize;

  m_ArchiveReader.ConfigureRawMemoryStreamReader(uiEntryIndex, pReader->m_MemStreamReader);

  if (pReader->Open(sArchivePath, this, FileShareMode).Failed())
  {
    NS_DEFAULT_DELETE(pReader);
    return nullptr;
  }

  return pReader;
}

void nsDataDirectory::ArchiveType::RemoveDataDirectory()
{
  ArchiveType* pThis = this;
  NS_DEFAULT_DELETE(pThis);
}

bool nsDataDirectory::ArchiveType::ExistsFile(nsStringView sFile, bool bOneSpecificDataDir)
{
  nsStringBuilder sArchivePath = m_sArchiveSubFolder;
  sArchivePath.AppendPath(sFile);
  return m_ArchiveReader.GetArchiveTOC().FindEntry(sArchivePath) != nsInvalidIndex;
}

nsResult nsDataDirectory::ArchiveType::GetFileStats(nsStringView sFileOrFolder, bool bOneSpecificDataDir, nsFileStats& out_Stats)
{
  const nsArchiveTOC& toc = m_ArchiveReader.GetArchiveTOC();
  nsStringBuilder sArchivePath = m_sArchiveSubFolder;
  sArchivePath.AppendPath(sFileOrFolder);
  const nsUInt32 uiEntryIndex = toc.FindEntry(sArchivePath);

  if (uiEntryIndex == nsInvalidIndex)
    return NS_FAILURE;

  const nsArchiveEntry* pEntry = &toc.m_Entries[uiEntryIndex];

  const nsStringView sPath = toc.GetEntryPathString(uiEntryIndex);

  out_Stats.m_bIsDirectory = false;
  out_Stats.m_LastModificationTime = m_LastModificationTime;
  out_Stats.m_uiFileSize = pEntry->m_uiUncompressedDataSize;
  out_Stats.m_sParentPath = sPath;
  out_Stats.m_sParentPath.PathParentDirectory();
  out_Stats.m_sName = nsPathUtils::GetFileNameAndExtension(sPath);

  return NS_SUCCESS;
}

nsResult nsDataDirectory::ArchiveType::InternalInitializeDataDirectory(nsStringView sDirectory)
{
  nsStringBuilder sRedirected;
  NS_SUCCEED_OR_RETURN(nsFileSystem::ResolveSpecialDirectory(sDirectory, sRedirected));

  sRedirected.MakeCleanPath();
  // remove trailing slashes
  sRedirected.Trim("", "/");
  m_sRedirectedDataDirPath = sRedirected;

  bool bSupported = false;
  nsStringBuilder sArchivePath;

  nsHybridArray<nsString, 4, nsStaticAllocatorWrapper> extensions = nsArchiveUtils::GetAcceptedArchiveFileExtensions();


  for (const auto& ext : extensions)
  {
    const nsUInt32 uiLength = ext.GetElementCount();
    if (sRedirected.HasExtension(ext))
    {
      sArchivePath = sRedirected;
      m_sArchiveSubFolder = "";
      bSupported = true;
      goto endloop;
    }
    const char* szFound = nullptr;
    do
    {
      szFound = sRedirected.FindLastSubString_NoCase(ext, szFound);
      if (szFound != nullptr && szFound[uiLength] == '/')
      {
        sArchivePath = nsStringView(sRedirected.GetData(), szFound + uiLength);
        m_sArchiveSubFolder = szFound + uiLength + 1;
        bSupported = true;
        goto endloop;
      }

    } while (szFound != nullptr);
  }
endloop:
  if (!bSupported)
    return NS_FAILURE;

  nsFileStats stats;
  if (nsOSFile::GetFileStats(sArchivePath, stats).Failed())
    return NS_FAILURE;

  NS_LOG_BLOCK("nsArchiveDataDir", sDirectory);

  m_LastModificationTime = stats.m_LastModificationTime;

  NS_SUCCEED_OR_RETURN(m_ArchiveReader.OpenArchive(sArchivePath));

  ReloadExternalConfigs();

  return NS_SUCCESS;
}

void nsDataDirectory::ArchiveType::OnReaderWriterClose(nsDataDirectoryReaderWriterBase* pClosed)
{
  NS_LOCK(m_ReaderMutex);

  if (pClosed->GetDataDirUserData() == 0)
  {
    m_FreeReadersUncompressed.PushBack(static_cast<ArchiveReaderUncompressed*>(pClosed));
    return;
  }

#ifdef BUILDSYSTEM_ENABLE_ZSTD_SUPPORT
  if (pClosed->GetDataDirUserData() == 1)
  {
    m_FreeReadersZstd.PushBack(static_cast<ArchiveReaderZstd*>(pClosed));
    return;
  }
#endif


  NS_ASSERT_NOT_IMPLEMENTED;
}

//////////////////////////////////////////////////////////////////////////

nsDataDirectory::ArchiveReaderUncompressed::ArchiveReaderUncompressed(nsInt32 iDataDirUserData)
  : nsDataDirectoryReader(iDataDirUserData)
{
}

nsDataDirectory::ArchiveReaderUncompressed::~ArchiveReaderUncompressed() = default;

nsUInt64 nsDataDirectory::ArchiveReaderUncompressed::Read(void* pBuffer, nsUInt64 uiBytes)
{
  return m_MemStreamReader.ReadBytes(pBuffer, uiBytes);
}

nsUInt64 nsDataDirectory::ArchiveReaderUncompressed::GetFileSize() const
{
  return m_uiUncompressedSize;
}

nsResult nsDataDirectory::ArchiveReaderUncompressed::InternalOpen(nsFileShareMode::Enum FileShareMode)
{
  NS_ASSERT_DEBUG(FileShareMode != nsFileShareMode::Exclusive, "Archives only support shared reading of files. Exclusive access cannot be guaranteed.");

  // nothing to do
  return NS_SUCCESS;
}

void nsDataDirectory::ArchiveReaderUncompressed::InternalClose()
{
  // nothing to do
}

//////////////////////////////////////////////////////////////////////////

#ifdef BUILDSYSTEM_ENABLE_ZSTD_SUPPORT

nsDataDirectory::ArchiveReaderZstd::ArchiveReaderZstd(nsInt32 iDataDirUserData)
  : ArchiveReaderUncompressed(iDataDirUserData)
{
}

nsDataDirectory::ArchiveReaderZstd::~ArchiveReaderZstd() = default;

nsUInt64 nsDataDirectory::ArchiveReaderZstd::Read(void* pBuffer, nsUInt64 uiBytes)
{
  return m_CompressedStreamReader.ReadBytes(pBuffer, uiBytes);
}

nsResult nsDataDirectory::ArchiveReaderZstd::InternalOpen(nsFileShareMode::Enum FileShareMode)
{
  NS_ASSERT_DEBUG(FileShareMode != nsFileShareMode::Exclusive, "Archives only support shared reading of files. Exclusive access cannot be guaranteed.");

  m_CompressedStreamReader.SetInputStream(&m_MemStreamReader);
  return NS_SUCCESS;
}

#endif

//////////////////////////////////////////////////////////////////////////



NS_STATICLINK_FILE(Foundation, Foundation_IO_Archive_Implementation_DataDirTypeArchive);
