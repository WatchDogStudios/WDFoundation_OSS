/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/IO/FileSystem/FileSystem.h>
#include <Foundation/IO/OSFile.h>

nsResult nsDataDirectoryType::InitializeDataDirectory(nsStringView sDataDirPath)
{
  nsStringBuilder sPath = sDataDirPath;
  sPath.MakeCleanPath();

  NS_ASSERT_DEV(sPath.IsEmpty() || sPath.EndsWith("/"), "Data directory path must end with a slash.");

  m_sDataDirectoryPath = sPath;

  return InternalInitializeDataDirectory(m_sDataDirectoryPath.GetData());
}

bool nsDataDirectoryType::ExistsFile(nsStringView sFile, bool bOneSpecificDataDir)
{
  nsStringBuilder sRedirectedAsset;
  ResolveAssetRedirection(sFile, sRedirectedAsset);

  nsStringBuilder sPath = GetRedirectedDataDirectoryPath();
  sPath.AppendPath(sRedirectedAsset);
  return nsOSFile::ExistsFile(sPath);
}

void nsDataDirectoryReaderWriterBase::Close()
{
  InternalClose();

  nsFileSystem::FileEvent fe;
  fe.m_EventType = nsFileSystem::FileEventType::CloseFile;
  fe.m_sFileOrDirectory = GetFilePath();
  fe.m_pDataDir = m_pDataDirectory;
  nsFileSystem::s_pData->m_Event.Broadcast(fe);

  m_pDataDirectory->OnReaderWriterClose(this);
}



NS_STATICLINK_FILE(Foundation, Foundation_IO_FileSystem_Implementation_DataDirType);
