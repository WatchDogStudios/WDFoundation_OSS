/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Math/Math.h>
#include <Foundation/Memory/AllocatorBase.h>
#include <Foundation/Memory/MemoryTracker.h>
#include <Foundation/Threading/ThreadUtils.h>

NS_MAKE_MEMBERFUNCTION_CHECKER(Reallocate, nsHasReallocate);

#include <Foundation/Memory/Implementation/Allocator_inl.h>

/// \brief Policy based allocator implementation of the nsAllocatorBase interface.
///
/// AllocationPolicy defines how the actual memory is allocated.\n
/// TrackingFlags defines how stats about allocations are tracked.\n
template <typename AllocationPolicy, nsUInt32 TrackingFlags = nsMemoryTrackingFlags::Default>
class nsAllocator : public nsInternal::nsAllocatorMixinReallocate<AllocationPolicy, TrackingFlags,
                      nsHasReallocate<AllocationPolicy, void* (AllocationPolicy::*)(void*, size_t, size_t, size_t)>::value>
{
public:
  nsAllocator(nsStringView sName, nsAllocatorBase* pParent = nullptr)
    : nsInternal::nsAllocatorMixinReallocate<AllocationPolicy, TrackingFlags,
        nsHasReallocate<AllocationPolicy, void* (AllocationPolicy::*)(void*, size_t, size_t, size_t)>::value>(sName, pParent)
  {
  }
};
