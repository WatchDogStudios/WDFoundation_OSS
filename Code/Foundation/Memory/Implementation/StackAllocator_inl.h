/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
template <nsUInt32 TrackingFlags>
nsStackAllocator<TrackingFlags>::nsStackAllocator(nsStringView sName, nsAllocatorBase* pParent)
  : nsAllocator<nsMemoryPolicies::nsStackAllocation, TrackingFlags>(sName, pParent)
  , m_DestructData(pParent)
  , m_PtrToDestructDataIndexTable(pParent)
{
}

template <nsUInt32 TrackingFlags>
nsStackAllocator<TrackingFlags>::~nsStackAllocator()
{
  Reset();
}

template <nsUInt32 TrackingFlags>
void* nsStackAllocator<TrackingFlags>::Allocate(size_t uiSize, size_t uiAlign, nsMemoryUtils::DestructorFunction destructorFunc)
{
  NS_LOCK(m_Mutex);

  void* ptr = nsAllocator<nsMemoryPolicies::nsStackAllocation, TrackingFlags>::Allocate(uiSize, uiAlign, destructorFunc);

  if (destructorFunc != nullptr)
  {
    nsUInt32 uiIndex = m_DestructData.GetCount();
    m_PtrToDestructDataIndexTable.Insert(ptr, uiIndex);

    auto& data = m_DestructData.ExpandAndGetRef();
    data.m_Func = destructorFunc;
    data.m_Ptr = ptr;
  }

  return ptr;
}

template <nsUInt32 TrackingFlags>
void nsStackAllocator<TrackingFlags>::Deallocate(void* pPtr)
{
  NS_LOCK(m_Mutex);

  nsUInt32 uiIndex;
  if (m_PtrToDestructDataIndexTable.Remove(pPtr, &uiIndex))
  {
    auto& data = m_DestructData[uiIndex];
    data.m_Func = nullptr;
    data.m_Ptr = nullptr;
  }

  nsAllocator<nsMemoryPolicies::nsStackAllocation, TrackingFlags>::Deallocate(pPtr);
}

NS_MSVC_ANALYSIS_WARNING_PUSH

// Disable warning for incorrect operator (compiler complains about the TrackingFlags bitwise and in the case that flags = None)
// even with the added guard of a check that it can't be 0.
NS_MSVC_ANALYSIS_WARNING_DISABLE(6313)

template <nsUInt32 TrackingFlags>
void nsStackAllocator<TrackingFlags>::Reset()
{
  NS_LOCK(m_Mutex);

  for (nsUInt32 i = m_DestructData.GetCount(); i-- > 0;)
  {
    auto& data = m_DestructData[i];
    if (data.m_Func != nullptr)
      data.m_Func(data.m_Ptr);
  }
  m_DestructData.Clear();
  m_PtrToDestructDataIndexTable.Clear();

  this->m_allocator.Reset();
  if ((TrackingFlags & nsMemoryTrackingFlags::EnableAllocationTracking) != 0)
  {
    nsMemoryTracker::RemoveAllAllocations(this->m_Id);
  }
  else if ((TrackingFlags & nsMemoryTrackingFlags::RegisterAllocator) != 0)
  {
    nsAllocatorBase::Stats stats;
    this->m_allocator.FillStats(stats);

    nsMemoryTracker::SetAllocatorStats(this->m_Id, stats);
  }
}
NS_MSVC_ANALYSIS_WARNING_POP
