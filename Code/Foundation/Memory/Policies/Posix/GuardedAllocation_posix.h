/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */

namespace nsMemoryPolicies
{
  nsGuardedAllocation::nsGuardedAllocation(nsAllocatorBase* pParent) { NS_ASSERT_NOT_IMPLEMENTED; }

  void* nsGuardedAllocation::Allocate(size_t uiSize, size_t uiAlign)
  {
    NS_ASSERT_NOT_IMPLEMENTED;
    return nullptr;
  }

  void nsGuardedAllocation::Deallocate(void* ptr) { NS_ASSERT_NOT_IMPLEMENTED; }
} // namespace nsMemoryPolicies
