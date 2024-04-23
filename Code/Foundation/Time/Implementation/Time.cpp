/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Configuration/Startup.h>
#include <Foundation/Time/Time.h>

// clang-format off
NS_BEGIN_SUBSYSTEM_DECLARATION(Foundation, Time)

  // no dependencies

  ON_BASESYSTEMS_STARTUP
  {
    nsTime::Initialize();
  }

NS_END_SUBSYSTEM_DECLARATION;
// clang-format on

// Include inline file
#if NS_ENABLED(NS_PLATFORM_WINDOWS)
#  include <Foundation/Time/Implementation/Win/Time_win.h>
#elif NS_ENABLED(NS_PLATFORM_OSX)
#  include <Foundation/Time/Implementation/OSX/Time_osx.h>
#elif NS_ENABLED(NS_PLATFORM_LINUX) || NS_ENABLED(NS_PLATFORM_ANDROID)
#  include <Foundation/Time/Implementation/Posix/Time_posix.h>
#else
#  error "Time functions are not implemented on current platform"
#endif



NS_STATICLINK_FILE(Foundation, Foundation_Time_Implementation_Time);
