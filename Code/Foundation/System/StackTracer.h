/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Configuration/Plugin.h>
#include <Foundation/Types/Delegate.h>

/// \brief Helper class to capture the current stack and print a captured stack
class NS_FOUNDATION_DLL nsStackTracer
{
public:
  /// \brief Captures the current stack trace.
  ///
  /// The trace will contain not more than ref_trace.GetCount() entries.
  /// [Windows] If called in an exception handler, set pContext to PEXCEPTION_POINTERS::ContextRecord.
  /// Returns the actual number of captured entries.
  static nsUInt32 GetStackTrace(nsArrayPtr<void*>& ref_trace, void* pContext = nullptr);

  /// \brief Callback-function to print a text somewhere
  using PrintFunc = nsDelegate<void(const char* szText)>;

  /// \brief Print a stack trace
  static void ResolveStackTrace(const nsArrayPtr<void*>& trace, PrintFunc printFunc);

  /// \brief Print a stack trace without resolving it
  static void PrintStackTrace(const nsArrayPtr<void*>& trace, PrintFunc printFunc);

private:
  nsStackTracer() = delete;

  static void OnPluginEvent(const nsPluginEvent& e);

  NS_MAKE_SUBSYSTEM_STARTUP_FRIEND(Foundation, StackTracer);
};
