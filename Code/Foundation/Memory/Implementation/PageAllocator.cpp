/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Memory/MemoryTracker.h>
#include <Foundation/Memory/PageAllocator.h>
#include <Foundation/System/SystemInformation.h>

static nsAllocatorId GetPageAllocatorId()
{
  static nsAllocatorId id;

  if (id.IsInvalidated())
  {
    id = nsMemoryTracker::RegisterAllocator("Page", nsMemoryTrackingFlags::Default, nsAllocatorId());
  }

  return id;
}

nsAllocatorId nsPageAllocator::GetId()
{
  return GetPageAllocatorId();
}

#if NS_ENABLED(NS_PLATFORM_WINDOWS)
#  include <Foundation/Memory/Implementation/Win/PageAllocator_win.h>
#elif NS_ENABLED(NS_PLATFORM_OSX) || NS_ENABLED(NS_PLATFORM_LINUX) || NS_ENABLED(NS_PLATFORM_ANDROID)
#  include <Foundation/Memory/Implementation/Posix/PageAllocator_posix.h>
#else
#  error "nsPageAllocator is not implemented on current platform"
#endif

NS_STATICLINK_FILE(Foundation, Foundation_Memory_Implementation_PageAllocator);
