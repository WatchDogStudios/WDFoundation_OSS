/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Basics.h>

namespace nsMemoryPolicies
{
  /// \brief Default heap memory allocation policy.
  ///
  /// \see nsAllocator
  class nsHeapAllocation
  {
  public:
    NS_ALWAYS_INLINE nsHeapAllocation(nsAllocatorBase* pParent) {}
    NS_ALWAYS_INLINE ~nsHeapAllocation() = default;

    NS_FORCE_INLINE void* Allocate(size_t uiSize, size_t uiAlign)
    {
      // malloc has no alignment guarantees, even though on many systems it returns 16 byte aligned data
      // if these asserts fail, you need to check what container made the allocation and change it
      // to use an aligned allocator, e.g. nsAlignedAllocatorWrapper

      // unfortunately using NS_ALIGNMENT_MINIMUM doesn't work, because even on 32 Bit systems we try to do allocations with 8 Byte
      // alignment interestingly, the code that does that, seems to work fine anyway
      NS_ASSERT_DEBUG(
        uiAlign <= 8, "This allocator does not guarantee alignments larger than 8. Use an aligned allocator to allocate the desired data type.");

      void* ptr = malloc(PadSize(uiSize));
      NS_CHECK_ALIGNMENT(ptr, uiAlign);

      return OffsetPtr(ptr);
    }

    NS_FORCE_INLINE void* Reallocate(void* pCurrentPtr, size_t uiCurrentSize, size_t uiNewSize, size_t uiAlign)
    {
      void* ptr = realloc(RestorePtr(pCurrentPtr), PadSize(uiNewSize));
      NS_CHECK_ALIGNMENT(ptr, uiAlign);

      return OffsetPtr(ptr);
    }

    NS_ALWAYS_INLINE void Deallocate(void* pPtr) { free(RestorePtr(pPtr)); }

    NS_ALWAYS_INLINE nsAllocatorBase* GetParent() const { return nullptr; }

  private:
    NS_ALWAYS_INLINE size_t PadSize(size_t uiSize)
    {
#if NS_ENABLED(NS_COMPILE_FOR_DEBUG)
      return uiSize + 2 * NS_ALIGNMENT_MINIMUM;
#else
      return uiSize;
#endif
    }

    NS_ALWAYS_INLINE void* OffsetPtr(void* ptr)
    {
#if NS_ENABLED(NS_COMPILE_FOR_DEBUG)
      nsUInt32 uiOffset = nsMemoryUtils::IsAligned(ptr, 2 * NS_ALIGNMENT_MINIMUM) ? NS_ALIGNMENT_MINIMUM : 2 * NS_ALIGNMENT_MINIMUM;
      ptr = nsMemoryUtils::AddByteOffset(ptr, uiOffset - 4);
      *static_cast<nsUInt32*>(ptr) = uiOffset;
      return nsMemoryUtils::AddByteOffset(ptr, 4);
#else
      return ptr;
#endif
    }

    NS_ALWAYS_INLINE void* RestorePtr(void* ptr)
    {
#if NS_ENABLED(NS_COMPILE_FOR_DEBUG)
      ptr = nsMemoryUtils::AddByteOffset(ptr, -4);
      nsInt32 uiOffset = *static_cast<nsUInt32*>(ptr);
      return nsMemoryUtils::AddByteOffset(ptr, -uiOffset + 4);
#else
      return ptr;
#endif
    }
  };
} // namespace nsMemoryPolicies
