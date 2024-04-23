/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#if NS_ENABLED(NS_PLATFORM_WINDOWS_DESKTOP)
#  include <Foundation/Application/Implementation/Win/ApplicationEntryPoint_win.h>
#  include <Foundation/Basics/Platform/Win/IncludeWindows.h>

namespace nsApplicationDetails
{
  void SetConsoleCtrlHandler(nsMinWindows::BOOL(NS_WINDOWS_WINAPI* consoleHandler)(nsMinWindows::DWORD dwCtrlType))
  {
    ::SetConsoleCtrlHandler(consoleHandler, TRUE);
  }

  static nsMutex s_shutdownMutex;

  nsMutex& GetShutdownMutex()
  {
    return s_shutdownMutex;
  }

} // namespace nsApplicationDetails
#endif


NS_STATICLINK_FILE(Foundation, Foundation_Application_Implementation_Win_ApplicationEntryPoint_win);
