/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#if NS_ENABLED(NS_PLATFORM_WINDOWS_UWP)
#  include <Foundation/Application/Implementation/Uwp/ApplicationEntryPoint_uwp.h>
#  include <roapi.h>

namespace nsApplicationDetails
{
  nsResult InitializeWinrt()
  {
    HRESULT result = RoInitialize(RO_INIT_MULTITHREADED);
    if (FAILED(result))
    {
      nsLog::Printf("Failed to init WinRT: %i", result);
      return NS_FAILURE;
    }

    return NS_SUCCESS;
  }

  void UninitializeWinrt() { RoUninitialize(); }
} // namespace nsApplicationDetails
#endif


NS_STATICLINK_FILE(Foundation, Foundation_Application_Implementation_uwp_ApplicationEntryPoint_uwp);
