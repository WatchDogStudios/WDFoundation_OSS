/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */

NS_FORCE_INLINE void* nsAlignedHeapAllocation::Allocate(size_t uiSize, size_t uiAlign)
{
  uiAlign = nsMath::Max<size_t>(uiAlign, 16u);

  void* ptr = _aligned_malloc(uiSize, uiAlign);
  NS_CHECK_ALIGNMENT(ptr, uiAlign);

  return ptr;
}

NS_ALWAYS_INLINE void nsAlignedHeapAllocation::Deallocate(void* pPtr)
{
  _aligned_free(pPtr);
}
