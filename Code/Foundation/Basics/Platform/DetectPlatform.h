/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#if defined(_WINDOWS) || defined(_WIN32)
#  undef NS_PLATFORM_WINDOWS
#  define NS_PLATFORM_WINDOWS NS_ON

// further distinction between desktop, UWP etc. is done in Platform_win.h

#elif defined(__APPLE__) && defined(__MACH__)
#  include <TargetConditionals.h>

#  if TARGET_OS_MAC == 1
#    undef NS_PLATFORM_OSX
#    define NS_PLATFORM_OSX NS_ON
#  elif TARGET_OS_IPHONE == 1 || TARGET_IPHONE_SIMULATOR == 1
#    undef NS_PLATFORM_IOS
#    define NS_PLATFORM_IOS NS_ON
#  endif

#elif defined(ANDROID)

#  undef NS_PLATFORM_ANDROID
#  define NS_PLATFORM_ANDROID NS_ON

#elif defined(__linux)

#  undef NS_PLATFORM_LINUX
#  define NS_PLATFORM_LINUX NS_ON

//#elif defined(...)
//  #undef NS_PLATFORM_LINUX
//  #define NS_PLATFORM_LINUX NS_ON
#else
#  error "Unknown Platform."
#endif
