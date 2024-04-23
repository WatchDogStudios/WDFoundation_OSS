/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
namespace nsInternal
{
  template <typename AllocationPolicy, nsUInt32 TrackingFlags>
  class nsAllocatorImpl : public nsAllocatorBase
  {
  public:
    nsAllocatorImpl(nsStringView sName, nsAllocatorBase* pParent);
    ~nsAllocatorImpl();

    // nsAllocatorBase implementation
    virtual void* Allocate(size_t uiSize, size_t uiAlign, nsMemoryUtils::DestructorFunction destructorFunc = nullptr) override;
    virtual void Deallocate(void* pPtr) override;
    virtual size_t AllocatedSize(const void* pPtr) override;
    virtual nsAllocatorId GetId() const override;
    virtual Stats GetStats() const override;

    nsAllocatorBase* GetParent() const;

  protected:
    AllocationPolicy m_allocator;

    nsAllocatorId m_Id;
    nsThreadID m_ThreadID;
  };

  template <typename AllocationPolicy, nsUInt32 TrackingFlags, bool HasReallocate>
  class nsAllocatorMixinReallocate : public nsAllocatorImpl<AllocationPolicy, TrackingFlags>
  {
  public:
    nsAllocatorMixinReallocate(nsStringView sName, nsAllocatorBase* pParent);
  };

  template <typename AllocationPolicy, nsUInt32 TrackingFlags>
  class nsAllocatorMixinReallocate<AllocationPolicy, TrackingFlags, true> : public nsAllocatorImpl<AllocationPolicy, TrackingFlags>
  {
  public:
    nsAllocatorMixinReallocate(nsStringView sName, nsAllocatorBase* pParent);
    virtual void* Reallocate(void* pPtr, size_t uiCurrentSize, size_t uiNewSize, size_t uiAlign) override;
  };
}; // namespace nsInternal

template <typename A, nsUInt32 TrackingFlags>
NS_FORCE_INLINE nsInternal::nsAllocatorImpl<A, TrackingFlags>::nsAllocatorImpl(nsStringView sName, nsAllocatorBase* pParent /* = nullptr */)
  : m_allocator(pParent)
  , m_ThreadID(nsThreadUtils::GetCurrentThreadID())
{
  if ((TrackingFlags & nsMemoryTrackingFlags::RegisterAllocator) != 0)
  {
    NS_CHECK_AT_COMPILETIME_MSG((TrackingFlags & ~nsMemoryTrackingFlags::All) == 0, "Invalid tracking flags");
    const nsUInt32 uiTrackingFlags = TrackingFlags;
    nsBitflags<nsMemoryTrackingFlags> flags = *reinterpret_cast<const nsBitflags<nsMemoryTrackingFlags>*>(&uiTrackingFlags);
    this->m_Id = nsMemoryTracker::RegisterAllocator(sName, flags, pParent != nullptr ? pParent->GetId() : nsAllocatorId());
  }
}

template <typename A, nsUInt32 TrackingFlags>
nsInternal::nsAllocatorImpl<A, TrackingFlags>::~nsAllocatorImpl()
{
  // NS_ASSERT_RELEASE(m_ThreadID == nsThreadUtils::GetCurrentThreadID(), "Allocator is deleted from another thread");

  if ((TrackingFlags & nsMemoryTrackingFlags::RegisterAllocator) != 0)
  {
    nsMemoryTracker::DeregisterAllocator(this->m_Id);
  }
}

template <typename A, nsUInt32 TrackingFlags>
void* nsInternal::nsAllocatorImpl<A, TrackingFlags>::Allocate(size_t uiSize, size_t uiAlign, nsMemoryUtils::DestructorFunction destructorFunc)
{
  // zero size allocations always return nullptr without tracking (since deallocate nullptr is ignored)
  if (uiSize == 0)
    return nullptr;

  NS_ASSERT_DEBUG(nsMath::IsPowerOf2((nsUInt32)uiAlign), "Alignment must be power of two");

  nsTime fAllocationTime = nsTime::Now();

  void* ptr = m_allocator.Allocate(uiSize, uiAlign);
  NS_ASSERT_DEV(ptr != nullptr, "Could not allocate {0} bytes. Out of memory?", uiSize);

  if ((TrackingFlags & nsMemoryTrackingFlags::EnableAllocationTracking) != 0)
  {
    nsBitflags<nsMemoryTrackingFlags> flags;
    flags.SetValue(TrackingFlags);

    nsMemoryTracker::AddAllocation(this->m_Id, flags, ptr, uiSize, uiAlign, nsTime::Now() - fAllocationTime);
  }

  return ptr;
}

template <typename A, nsUInt32 TrackingFlags>
void nsInternal::nsAllocatorImpl<A, TrackingFlags>::Deallocate(void* pPtr)
{
  if ((TrackingFlags & nsMemoryTrackingFlags::EnableAllocationTracking) != 0)
  {
    nsMemoryTracker::RemoveAllocation(this->m_Id, pPtr);
  }

  m_allocator.Deallocate(pPtr);
}

template <typename A, nsUInt32 TrackingFlags>
size_t nsInternal::nsAllocatorImpl<A, TrackingFlags>::AllocatedSize(const void* pPtr)
{
  if ((TrackingFlags & nsMemoryTrackingFlags::EnableAllocationTracking) != 0)
  {
    return nsMemoryTracker::GetAllocationInfo(this->m_Id, pPtr).m_uiSize;
  }

  return 0;
}

template <typename A, nsUInt32 TrackingFlags>
nsAllocatorId nsInternal::nsAllocatorImpl<A, TrackingFlags>::GetId() const
{
  return this->m_Id;
}

template <typename A, nsUInt32 TrackingFlags>
nsAllocatorBase::Stats nsInternal::nsAllocatorImpl<A, TrackingFlags>::GetStats() const
{
  if ((TrackingFlags & nsMemoryTrackingFlags::RegisterAllocator) != 0)
  {
    return nsMemoryTracker::GetAllocatorStats(this->m_Id);
  }

  return Stats();
}

template <typename A, nsUInt32 TrackingFlags>
NS_ALWAYS_INLINE nsAllocatorBase* nsInternal::nsAllocatorImpl<A, TrackingFlags>::GetParent() const
{
  return m_allocator.GetParent();
}

template <typename A, nsUInt32 TrackingFlags, bool HasReallocate>
nsInternal::nsAllocatorMixinReallocate<A, TrackingFlags, HasReallocate>::nsAllocatorMixinReallocate(nsStringView sName, nsAllocatorBase* pParent)
  : nsAllocatorImpl<A, TrackingFlags>(sName, pParent)
{
}

template <typename A, nsUInt32 TrackingFlags>
nsInternal::nsAllocatorMixinReallocate<A, TrackingFlags, true>::nsAllocatorMixinReallocate(nsStringView sName, nsAllocatorBase* pParent)
  : nsAllocatorImpl<A, TrackingFlags>(sName, pParent)
{
}

template <typename A, nsUInt32 TrackingFlags>
void* nsInternal::nsAllocatorMixinReallocate<A, TrackingFlags, true>::Reallocate(void* pPtr, size_t uiCurrentSize, size_t uiNewSize, size_t uiAlign)
{
  if ((TrackingFlags & nsMemoryTrackingFlags::EnableAllocationTracking) != 0)
  {
    nsMemoryTracker::RemoveAllocation(this->m_Id, pPtr);
  }

  nsTime fAllocationTime = nsTime::Now();

  void* pNewMem = this->m_allocator.Reallocate(pPtr, uiCurrentSize, uiNewSize, uiAlign);

  if ((TrackingFlags & nsMemoryTrackingFlags::EnableAllocationTracking) != 0)
  {
    nsBitflags<nsMemoryTrackingFlags> flags;
    flags.SetValue(TrackingFlags);

    nsMemoryTracker::AddAllocation(this->m_Id, flags, pNewMem, uiNewSize, uiAlign, nsTime::Now() - fAllocationTime);
  }
  return pNewMem;
}
