/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Basics.h>

namespace nsMemoryPolicies
{
  /// \brief Aligned Heap memory allocation policy.
  ///
  /// \see nsAllocator
  class nsAlignedHeapAllocation
  {
  public:
    NS_ALWAYS_INLINE nsAlignedHeapAllocation(nsAllocatorBase* pParent) {}
    NS_ALWAYS_INLINE ~nsAlignedHeapAllocation() = default;

    void* Allocate(size_t uiSize, size_t uiAlign);
    void Deallocate(void* pPtr);

    NS_ALWAYS_INLINE nsAllocatorBase* GetParent() const { return nullptr; }
  };

#if NS_ENABLED(NS_PLATFORM_WINDOWS)
#  include <Foundation/Memory/Policies/Win/AlignedHeapAllocation_win.h>
#elif NS_ENABLED(NS_PLATFORM_OSX) || NS_ENABLED(NS_PLATFORM_LINUX) || NS_ENABLED(NS_PLATFORM_ANDROID)
#  include <Foundation/Memory/Policies/Posix/AlignedHeapAllocation_posix.h>
#else
#  error "nsAlignedHeapAllocation is not implemented on current platform"
#endif
} // namespace nsMemoryPolicies
