/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Containers/StaticRingBuffer.h>
#include <Foundation/Math/Math.h>
#include <Foundation/Threading/Lock.h>
#include <Foundation/Threading/Mutex.h>

namespace nsMemoryPolicies
{
  class nsGuardedAllocation
  {
  public:
    nsGuardedAllocation(nsAllocatorBase* pParent);
    NS_ALWAYS_INLINE ~nsGuardedAllocation() = default;

    void* Allocate(size_t uiSize, size_t uiAlign);
    void Deallocate(void* pPtr);

    NS_ALWAYS_INLINE nsAllocatorBase* GetParent() const { return nullptr; }

  private:
    nsMutex m_Mutex;

    nsUInt32 m_uiPageSize;

    nsStaticRingBuffer<void*, (1 << 16)> m_AllocationsToFreeLater;
  };
} // namespace nsMemoryPolicies
