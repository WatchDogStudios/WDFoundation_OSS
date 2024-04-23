/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#if NS_ENABLED(NS_PLATFORM_WINDOWS)
#  include <Foundation/Basics/Platform/Win/PlatformFeatures_win.h>
#elif NS_ENABLED(NS_PLATFORM_OSX)
#  include <Foundation/Basics/Platform/OSX/PlatformFeatures_OSX.h>
#elif NS_ENABLED(NS_PLATFORM_LINUX)
#  include <Foundation/Basics/Platform/Linux/PlatformFeatures_Linux.h>
#elif NS_ENABLED(NS_PLATFORM_ANDROID)
#  include <Foundation/Basics/Platform/Android/PlatformFeatures_Android.h>
#else
#  error "Undefined platform!"
#endif


// now check that the defines for each feature are set (either to 1 or 0, but they must be defined)

#ifndef NS_SUPPORTS_FILE_ITERATORS
#  error "NS_SUPPORTS_FILE_ITERATORS is not defined."
#endif

#ifndef NS_USE_POSIX_FILE_API
#  error "NS_USE_POSIX_FILE_API is not defined."
#endif

#ifndef NS_SUPPORTS_FILE_STATS
#  error "NS_SUPPORTS_FILE_STATS is not defined."
#endif

#ifndef NS_SUPPORTS_MEMORY_MAPPED_FILE
#  error "NS_SUPPORTS_MEMORY_MAPPED_FILE is not defined."
#endif

#ifndef NS_SUPPORTS_SHARED_MEMORY
#  error "NS_SUPPORTS_SHARED_MEMORY is not defined."
#endif

#ifndef NS_SUPPORTS_DYNAMIC_PLUGINS
#  error "NS_SUPPORTS_DYNAMIC_PLUGINS is not defined."
#endif

#ifndef NS_SUPPORTS_UNRESTRICTED_FILE_ACCESS
#  error "NS_SUPPORTS_UNRESTRICTED_FILE_ACCESS is not defined."
#endif

#ifndef NS_SUPPORTS_CASE_INSENSITIVE_PATHS
#  error "NS_SUPPORTS_CASE_INSENSITIVE_PATHS is not defined."
#endif

#ifndef NS_SUPPORTS_LONG_PATHS
#  error "NS_SUPPORTS_LONG_PATHS is not defined."
#endif
