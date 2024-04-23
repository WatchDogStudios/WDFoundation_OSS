/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

#include <Foundation/Basics/Platform/Win/Platform_win.h>
#include <Foundation/Time/Time.h>

// static
void* nsPageAllocator::AllocatePage(size_t uiSize)
{
  nsTime fAllocationTime = nsTime::Now();

  void* ptr = ::VirtualAlloc(nullptr, uiSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  NS_ASSERT_DEV(ptr != nullptr, "Could not allocate memory pages. Error Code '{0}'", nsArgErrorCode(::GetLastError()));

  size_t uiAlign = nsSystemInformation::Get().GetMemoryPageSize();
  NS_CHECK_ALIGNMENT(ptr, uiAlign);

  if ((nsMemoryTrackingFlags::Default & nsMemoryTrackingFlags::EnableAllocationTracking) != 0)
  {
    nsMemoryTracker::AddAllocation(GetPageAllocatorId(), nsMemoryTrackingFlags::Default, ptr, uiSize, uiAlign, nsTime::Now() - fAllocationTime);
  }

  return ptr;
}

// static
void nsPageAllocator::DeallocatePage(void* pPtr)
{
  if ((nsMemoryTrackingFlags::Default & nsMemoryTrackingFlags::EnableAllocationTracking) != 0)
  {
    nsMemoryTracker::RemoveAllocation(GetPageAllocatorId(), pPtr);
  }

  NS_VERIFY(::VirtualFree(pPtr, 0, MEM_RELEASE), "Could not free memory pages. Error Code '{0}'", nsArgErrorCode(::GetLastError()));
}
