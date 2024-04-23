/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#if NS_ENABLED(NS_PLATFORM_WINDOWS)
#  include <Foundation/System/Implementation/Win/MiniDumpUtils_win.h>
#elif NS_ENABLED(NS_PLATFORM_OSX)
#  include <Foundation/System/Implementation/OSX/MiniDumpUtils_OSX.h>
#elif NS_ENABLED(NS_PLATFORM_LINUX) || NS_ENABLED(NS_PLATFORM_ANDROID)
#  include <Foundation/System/Implementation/Posix/MiniDumpUtils_posix.h>
#else
#  error "Mini-dump functions are not implemented on current platform"
#endif



NS_STATICLINK_FILE(Foundation, Foundation_System_Implementation_MiniDumpUtils);
