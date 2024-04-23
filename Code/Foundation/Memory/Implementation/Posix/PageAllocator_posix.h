/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */

#include <Foundation/Time/Time.h>

// static
void* nsPageAllocator::AllocatePage(size_t uiSize)
{
  nsTime fAllocationTime = nsTime::Now();

  void* ptr = nullptr;
  size_t uiAlign = nsSystemInformation::Get().GetMemoryPageSize();
  const int res = posix_memalign(&ptr, uiAlign, uiSize);
  NS_ASSERT_DEBUG(res == 0, "Failed to align pointer");
  NS_IGNORE_UNUSED(res);

  NS_CHECK_ALIGNMENT(ptr, uiAlign);

  if ((nsMemoryTrackingFlags::Default & nsMemoryTrackingFlags::EnableAllocationTracking) != 0)
  {
    nsMemoryTracker::AddAllocation(GetPageAllocatorId(), nsMemoryTrackingFlags::Default, ptr, uiSize, uiAlign, nsTime::Now() - fAllocationTime);
  }

  return ptr;
}

// static
void nsPageAllocator::DeallocatePage(void* ptr)
{
  if ((nsMemoryTrackingFlags::Default & nsMemoryTrackingFlags::EnableAllocationTracking) != 0)
  {
    nsMemoryTracker::RemoveAllocation(GetPageAllocatorId(), ptr);
  }

  free(ptr);
}
