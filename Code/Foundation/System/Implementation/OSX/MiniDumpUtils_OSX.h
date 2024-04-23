/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

#include <Foundation/System/MiniDumpUtils.h>

nsStatus nsMiniDumpUtils::WriteExternalProcessMiniDump(nsStringView sDumpFile, nsUInt32 uiProcessID)
{
  return nsStatus("Not implemented on OSX");
}

nsStatus nsMiniDumpUtils::LaunchMiniDumpTool(nsStringView sDumpFile)
{
  return nsStatus("Not implemented on OSX");
}
