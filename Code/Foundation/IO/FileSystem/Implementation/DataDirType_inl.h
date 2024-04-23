/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Strings/StringBuilder.h>

inline nsDataDirectoryReaderWriterBase::nsDataDirectoryReaderWriterBase(nsInt32 iDataDirUserData, bool bIsReader)
{
  m_iDataDirUserData = iDataDirUserData;
  m_pDataDirectory = nullptr;
  m_bIsReader = bIsReader;
}

inline nsResult nsDataDirectoryReaderWriterBase::Open(nsStringView sFile, nsDataDirectoryType* pDataDirectory, nsFileShareMode::Enum fileShareMode)
{
  m_pDataDirectory = pDataDirectory;
  m_sFilePath = sFile;

  return InternalOpen(fileShareMode);
}

inline const nsString128& nsDataDirectoryReaderWriterBase::GetFilePath() const
{
  return m_sFilePath;
}

inline nsDataDirectoryType* nsDataDirectoryReaderWriterBase::GetDataDirectory() const
{
  return m_pDataDirectory;
}
