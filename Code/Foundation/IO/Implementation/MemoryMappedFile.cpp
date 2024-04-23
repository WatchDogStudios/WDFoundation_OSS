/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#if NS_ENABLED(NS_PLATFORM_WINDOWS_DESKTOP)
#  include <Foundation/IO/Implementation/Win/MemoryMappedFile_win.h>
#elif NS_ENABLED(NS_PLATFORM_WINDOWS_UWP)
#  include <Foundation/IO/Implementation/Win/MemoryMappedFile_uwp.h>
#elif NS_ENABLED(NS_USE_POSIX_FILE_API)
#  include <Foundation/IO/Implementation/Posix/MemoryMappedFile_posix.h>
#else
#  error "Unknown Platform."
#endif


NS_STATICLINK_FILE(Foundation, Foundation_IO_Implementation_MemoryMappedFile);
