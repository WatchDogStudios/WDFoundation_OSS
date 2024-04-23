/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Threading/Semaphore.h>

#if NS_ENABLED(NS_PLATFORM_WINDOWS)
#  include <Foundation/Threading/Implementation/Win/Semaphore_win.h>
#elif NS_ENABLED(NS_PLATFORM_OSX) || NS_ENABLED(NS_PLATFORM_LINUX) || NS_ENABLED(NS_PLATFORM_ANDROID)
#  include <Foundation/Threading/Implementation/Posix/Semaphore_posix.h>
#else
#  error "Semaphore is not implemented on current platform"
#endif


NS_STATICLINK_FILE(Foundation, Foundation_Threading_Implementation_Semaphore);
