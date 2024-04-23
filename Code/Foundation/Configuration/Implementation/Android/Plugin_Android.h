/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

#include <Foundation/Configuration/Plugin.h>

using nsPluginModule = void*;

void nsPlugin::GetPluginPaths(nsStringView sPluginName, nsStringBuilder& sOriginalFile, nsStringBuilder& sCopiedFile, nsUInt8 uiFileCopyNumber)
{
  NS_ASSERT_NOT_IMPLEMENTED;
}

nsResult UnloadPluginModule(nsPluginModule& Module, nsStringView sPluginFile)
{
  NS_ASSERT_NOT_IMPLEMENTED;

  return NS_FAILURE;
}

nsResult LoadPluginModule(nsStringView sFileToLoad, nsPluginModule& Module, nsStringView sPluginFile)
{
  NS_ASSERT_NOT_IMPLEMENTED;
  return NS_FAILURE;
}
