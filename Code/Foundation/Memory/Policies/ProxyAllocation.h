/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Basics.h>

namespace nsMemoryPolicies
{
  /// \brief This Allocation policy redirects all operations to its parent.
  ///
  /// \note Note that the stats are taken on the proxy as well as on the parent.
  ///
  /// \see nsAllocator
  class nsProxyAllocation
  {
  public:
    NS_FORCE_INLINE nsProxyAllocation(nsAllocatorBase* pParent)
      : m_pParent(pParent)
    {
      NS_ASSERT_ALWAYS(m_pParent != nullptr, "Parent allocator must not be nullptr");
    }

    NS_FORCE_INLINE void* Allocate(size_t uiSize, size_t uiAlign) { return m_pParent->Allocate(uiSize, uiAlign); }

    NS_FORCE_INLINE void* Reallocate(void* pPtr, size_t uiCurrentSize, size_t uiNewSize, size_t uiAlign)
    {
      return m_pParent->Reallocate(pPtr, uiCurrentSize, uiNewSize, uiAlign);
    }

    NS_FORCE_INLINE void Deallocate(void* pPtr) { m_pParent->Deallocate(pPtr); }

    NS_FORCE_INLINE size_t AllocatedSize(const void* pPtr) { return m_pParent->AllocatedSize(pPtr); }

    NS_ALWAYS_INLINE nsAllocatorBase* GetParent() const { return m_pParent; }

  private:
    nsAllocatorBase* m_pParent;
  };
} // namespace nsMemoryPolicies
