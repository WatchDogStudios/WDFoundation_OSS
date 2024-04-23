/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Basics.h>

#include <Foundation/Basics/Platform/Win/MinWindows.h>
#include <Foundation/Types/Status.h>

#if NS_ENABLED(NS_PLATFORM_WINDOWS)
extern "C"
{
  struct _EXCEPTION_POINTERS;
}
#endif

/// \brief Functionality for writing process mini-dumps (callstacks, memory snapshots, etc)
struct NS_FOUNDATION_DLL nsMiniDumpUtils
{
  /// \brief Tries to write a mini-dump for the external process with the given process ID.
  ///
  /// \sa WriteProcessMiniDump()
  static nsStatus WriteExternalProcessMiniDump(nsStringView sDumpFile, nsUInt32 uiProcessID);

  /// \brief Tries to launch ns's 'MiniDumpTool' to write a mini-dump for THIS process (the recommended way when an application is crashing).
  ///
  /// \note On Windows: If the command line option '-fullcrashdumps' is specified, it is forwarded to the MiniDumpTool.
  static nsStatus LaunchMiniDumpTool(nsStringView sDumpFile);

#if NS_ENABLED(NS_PLATFORM_WINDOWS_DESKTOP)
  /// \brief Windows-specific implementation for writing a mini-dump of the running process.
  ///
  /// \sa WriteProcessMiniDump()
  static nsStatus WriteOwnProcessMiniDump(nsStringView sDumpFile, struct _EXCEPTION_POINTERS* pExceptionInfo);

  /// \brief Given a process ID this function tries to get a HANDLE to the process with the necessary access rights to write a mini-dump.
  static nsMinWindows::HANDLE GetProcessHandleWithNecessaryRights(nsUInt32 uiProcessID);

  /// \brief Windows-specific implementation for writing a mini-dump of another process.
  ///
  /// \sa WriteProcessMiniDump()
  static nsStatus WriteExternalProcessMiniDump(nsStringView sDumpFile, nsUInt32 uiProcessID, nsMinWindows::HANDLE hProcess);

  /// \brief Windows-specific implementation for writing a mini-dump of the running process.
  ///
  /// \note On Windows: If the command line option '-fullcrashdumps' is specified, a crash-dump with a full memory capture is made.
  static nsStatus WriteProcessMiniDump(nsStringView sDumpFile, nsUInt32 uiProcessID, nsMinWindows::HANDLE hProcess, struct _EXCEPTION_POINTERS* pExceptionInfo);

#endif
};
