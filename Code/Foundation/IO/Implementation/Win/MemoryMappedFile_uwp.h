/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/IO/MemoryMappedFile.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Strings/PathUtils.h>

///#TODO: Implement. Under some restrictions, UWP supports
/// CreateFileMappingFromApp, OpenFileMappingFromApp, MapViewOfFileFromApp
/// Needs adding codeGeneration capability to the app manifest.

struct nsMemoryMappedFileImpl
{
  nsMemoryMappedFile::Mode m_Mode = nsMemoryMappedFile::Mode::None;
  void* m_pMappedFilePtr = nullptr;
  nsUInt64 m_uiFileSize = 0;

  ~nsMemoryMappedFileImpl() {}
};

nsMemoryMappedFile::nsMemoryMappedFile()
{
  m_pImpl = NS_DEFAULT_NEW(nsMemoryMappedFileImpl);
}

nsMemoryMappedFile::~nsMemoryMappedFile()
{
  Close();
}

void nsMemoryMappedFile::Close()
{
  m_pImpl = NS_DEFAULT_NEW(nsMemoryMappedFileImpl);
}

nsMemoryMappedFile::Mode nsMemoryMappedFile::GetMode() const
{
  return m_pImpl->m_Mode;
}

const void* nsMemoryMappedFile::GetReadPointer(nsUInt64 uiOffset /*= 0*/, OffsetBase base /*= OffsetBase::Start*/) const
{
  NS_ASSERT_DEBUG(m_pImpl->m_Mode >= Mode::ReadOnly, "File must be opened with read access before accessing it for reading.");
  return m_pImpl->m_pMappedFilePtr;
}

void* nsMemoryMappedFile::GetWritePointer(nsUInt64 uiOffset /*= 0*/, OffsetBase base /*= OffsetBase::Start*/)
{
  NS_ASSERT_DEBUG(m_pImpl->m_Mode >= Mode::ReadWrite, "File must be opened with read/write access before accessing it for writing.");
  return m_pImpl->m_pMappedFilePtr;
}

nsUInt64 nsMemoryMappedFile::GetFileSize() const
{
  return m_pImpl->m_uiFileSize;
}
