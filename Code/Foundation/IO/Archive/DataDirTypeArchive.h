/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/IO/Archive/ArchiveReader.h>
#include <Foundation/IO/CompressedStreamZstd.h>
#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/FileSystem/Implementation/DataDirType.h>
#include <Foundation/IO/MemoryStream.h>
#include <Foundation/Time/Timestamp.h>

class nsArchiveEntry;

namespace nsDataDirectory
{
  class ArchiveReaderUncompressed;
  class ArchiveReaderZstd;
  class ArchiveReaderZip;

  class NS_FOUNDATION_DLL ArchiveType : public nsDataDirectoryType
  {
  public:
    ArchiveType();
    ~ArchiveType();

    static nsDataDirectoryType* Factory(nsStringView sDataDirectory, nsStringView sGroup, nsStringView sRootName, nsFileSystem::DataDirUsage usage);

    virtual const nsString128& GetRedirectedDataDirectoryPath() const override { return m_sRedirectedDataDirPath; }

  protected:
    virtual nsDataDirectoryReader* OpenFileToRead(nsStringView sFile, nsFileShareMode::Enum FileShareMode, bool bSpecificallyThisDataDir) override;

    virtual void RemoveDataDirectory() override;

    virtual bool ExistsFile(nsStringView sFile, bool bOneSpecificDataDir) override;

    virtual nsResult GetFileStats(nsStringView sFileOrFolder, bool bOneSpecificDataDir, nsFileStats& out_Stats) override;

    virtual nsResult InternalInitializeDataDirectory(nsStringView sDirectory) override;

    virtual void OnReaderWriterClose(nsDataDirectoryReaderWriterBase* pClosed) override;

    nsString128 m_sRedirectedDataDirPath;
    nsString32 m_sArchiveSubFolder;
    nsTimestamp m_LastModificationTime;
    nsArchiveReader m_ArchiveReader;

    nsMutex m_ReaderMutex;
    nsHybridArray<nsUniquePtr<ArchiveReaderUncompressed>, 4> m_ReadersUncompressed;
    nsHybridArray<ArchiveReaderUncompressed*, 4> m_FreeReadersUncompressed;

#ifdef BUILDSYSTEM_ENABLE_ZSTD_SUPPORT
    nsHybridArray<nsUniquePtr<ArchiveReaderZstd>, 4> m_ReadersZstd;
    nsHybridArray<ArchiveReaderZstd*, 4> m_FreeReadersZstd;
#endif
  };

  class NS_FOUNDATION_DLL ArchiveReaderUncompressed : public nsDataDirectoryReader
  {
    NS_DISALLOW_COPY_AND_ASSIGN(ArchiveReaderUncompressed);

  public:
    ArchiveReaderUncompressed(nsInt32 iDataDirUserData);
    ~ArchiveReaderUncompressed();

    virtual nsUInt64 Read(void* pBuffer, nsUInt64 uiBytes) override;
    virtual nsUInt64 GetFileSize() const override;

  protected:
    virtual nsResult InternalOpen(nsFileShareMode::Enum FileShareMode) override;
    virtual void InternalClose() override;

    friend class ArchiveType;

    nsUInt64 m_uiUncompressedSize = 0;
    nsUInt64 m_uiCompressedSize = 0;
    nsRawMemoryStreamReader m_MemStreamReader;
  };

#ifdef BUILDSYSTEM_ENABLE_ZSTD_SUPPORT
  class NS_FOUNDATION_DLL ArchiveReaderZstd : public ArchiveReaderUncompressed
  {
    NS_DISALLOW_COPY_AND_ASSIGN(ArchiveReaderZstd);

  public:
    ArchiveReaderZstd(nsInt32 iDataDirUserData);
    ~ArchiveReaderZstd();

    virtual nsUInt64 Read(void* pBuffer, nsUInt64 uiBytes) override;

  protected:
    virtual nsResult InternalOpen(nsFileShareMode::Enum FileShareMode) override;

    friend class ArchiveType;

    nsCompressedStreamReaderZstd m_CompressedStreamReader;
  };
#endif


} // namespace nsDataDirectory
