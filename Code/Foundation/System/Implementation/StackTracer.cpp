/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Configuration/Startup.h>
#include <Foundation/System/StackTracer.h>

// Include inline file
#if NS_ENABLED(NS_PLATFORM_WINDOWS)

#  if NS_ENABLED(NS_PLATFORM_WINDOWS_UWP)
#    include <Foundation/System/Implementation/Win/StackTracer_uwp.h>
#  else
#    include <Foundation/System/Implementation/Win/StackTracer_win.h>
#  endif

#elif NS_ENABLED(NS_PLATFORM_OSX) || NS_ENABLED(NS_PLATFORM_LINUX)
#  include <Foundation/System/Implementation/Posix/StackTracer_posix.h>
#elif NS_ENABLED(NS_PLATFORM_ANDROID)
#  include <Foundation/System/Implementation/Android/StackTracer_android.h>
#else
#  error "StackTracer is not implemented on current platform"
#endif

// clang-format off
NS_BEGIN_SUBSYSTEM_DECLARATION(Foundation, StackTracer)

  BEGIN_SUBSYSTEM_DEPENDENCIES
  "Time"
  END_SUBSYSTEM_DEPENDENCIES

  ON_CORESYSTEMS_STARTUP
  {
    nsPlugin::Events().AddEventHandler(nsStackTracer::OnPluginEvent);
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    nsPlugin::Events().RemoveEventHandler(nsStackTracer::OnPluginEvent);
  }

NS_END_SUBSYSTEM_DECLARATION;
// clang-format on

void nsStackTracer::PrintStackTrace(const nsArrayPtr<void*>& trace, nsStackTracer::PrintFunc printFunc)
{
  char buffer[32];
  const nsUInt32 uiNumTraceEntries = trace.GetCount();
  for (nsUInt32 i = 0; i < uiNumTraceEntries; i++)
  {
    nsStringUtils::snprintf(buffer, NS_ARRAY_SIZE(buffer), "%s%p", i == 0 ? "" : "|", trace[i]);
    printFunc(buffer);
  }
}

NS_STATICLINK_FILE(Foundation, Foundation_System_Implementation_StackTracer);
