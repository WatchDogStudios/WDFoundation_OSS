/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Containers/DynamicArray.h>
#include <Foundation/Containers/HashTable.h>
#include <Foundation/Memory/Allocator.h>
#include <Foundation/Memory/Policies/StackAllocation.h>
#include <Foundation/Threading/Lock.h>
#include <Foundation/Threading/Mutex.h>

template <nsUInt32 TrackingFlags = nsMemoryTrackingFlags::Default>
class nsStackAllocator : public nsAllocator<nsMemoryPolicies::nsStackAllocation, TrackingFlags>
{
public:
  nsStackAllocator(nsStringView sName, nsAllocatorBase* pParent);
  ~nsStackAllocator();

  virtual void* Allocate(size_t uiSize, size_t uiAlign, nsMemoryUtils::DestructorFunction destructorFunc) override;
  virtual void Deallocate(void* pPtr) override;

  /// \brief
  ///   Resets the allocator freeing all memory.
  void Reset();

private:
  struct DestructData
  {
    NS_DECLARE_POD_TYPE();

    nsMemoryUtils::DestructorFunction m_Func;
    void* m_Ptr;
  };

  nsMutex m_Mutex;
  nsDynamicArray<DestructData> m_DestructData;
  nsHashTable<void*, nsUInt32> m_PtrToDestructDataIndexTable;
};

#include <Foundation/Memory/Implementation/StackAllocator_inl.h>
