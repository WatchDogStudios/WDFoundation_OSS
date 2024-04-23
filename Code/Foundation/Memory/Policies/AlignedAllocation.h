/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Math/Math.h>

namespace nsMemoryPolicies
{
  /// \brief Allocation policy to support custom alignment per allocation.
  ///
  /// \see nsAllocator
  template <typename T>
  class nsAlignedAllocation
  {
  public:
    nsAlignedAllocation(nsAllocatorBase* pParent)
      : m_allocator(pParent)
    {
    }

    void* Allocate(size_t uiSize, size_t uiAlign)
    {
      NS_ASSERT_DEV(uiAlign < (1 << 24), "Alignment of {0} is too big. Maximum supported alignment is 16MB.", uiAlign);

      const nsUInt32 uiPadding = (nsUInt32)(uiAlign - 1 + MetadataSize);
      const size_t uiAlignedSize = uiSize + uiPadding;

      nsUInt8* pMemory = (nsUInt8*)m_allocator.Allocate(uiAlignedSize, NS_ALIGNMENT_MINIMUM);

      nsUInt8* pAlignedMemory = nsMemoryUtils::AlignBackwards(pMemory + uiPadding, uiAlign);

      nsUInt32* pMetadata = GetMetadataPtr(pAlignedMemory);
      *pMetadata = PackMetadata((nsUInt32)(pAlignedMemory - pMemory), (nsUInt32)uiAlign);

      return pAlignedMemory;
    }

    void Deallocate(void* pPtr)
    {
      const nsUInt32 uiOffset = UnpackOffset(GetMetadata(pPtr));
      nsUInt8* pMemory = static_cast<nsUInt8*>(pPtr) - uiOffset;
      m_allocator.Deallocate(pMemory);
    }

    size_t AllocatedSize(const void* pPtr)
    {
      const nsUInt32 uiMetadata = GetMetadata(pPtr);
      const nsUInt32 uiOffset = UnpackOffset(uiMetadata);
      const nsUInt32 uiAlign = UnpackAlignment(uiMetadata);
      const nsUInt32 uiPadding = uiAlign - 1 + MetadataSize;

      const nsUInt8* pMemory = static_cast<const nsUInt8*>(pPtr) - uiOffset;
      return m_allocator.AllocatedSize(pMemory) - uiPadding;
    }

    size_t UsedMemorySize(const void* pPtr)
    {
      const nsUInt32 uiOffset = UnpackOffset(GetMetadata(pPtr));
      const nsUInt8* pMemory = static_cast<const nsUInt8*>(pPtr) - uiOffset;
      return m_allocator.UsedMemorySize(pMemory);
    }

    NS_ALWAYS_INLINE nsAllocatorBase* GetParent() const { return m_allocator.GetParent(); }

  private:
    enum
    {
      MetadataSize = sizeof(nsUInt32)
    };

    // Meta-data is stored 4 bytes before the aligned memory
    inline nsUInt32* GetMetadataPtr(void* pAlignedMemory)
    {
      return static_cast<nsUInt32*>(nsMemoryUtils::AddByteOffset(pAlignedMemory, -MetadataSize));
    }

    inline nsUInt32 GetMetadata(const void* pAlignedMemory)
    {
      return *static_cast<const nsUInt32*>(nsMemoryUtils::AddByteOffset(pAlignedMemory, -MetadataSize));
    }

    // Store offset between pMemory and pAlignedMemory in the lower 24 bit of meta-data.
    // The upper 8 bit are used to store the Log2 of the alignment.
    NS_ALWAYS_INLINE nsUInt32 PackMetadata(nsUInt32 uiOffset, nsUInt32 uiAlignment) { return uiOffset | (nsMath::Log2i(uiAlignment) << 24); }

    NS_ALWAYS_INLINE nsUInt32 UnpackOffset(nsUInt32 uiMetadata) { return uiMetadata & 0x00FFFFFF; }

    NS_ALWAYS_INLINE nsUInt32 UnpackAlignment(nsUInt32 uiMetadata) { return 1 << (uiMetadata >> 24); }

    T m_allocator;
  };
} // namespace nsMemoryPolicies
