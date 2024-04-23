/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Time/Time.h>
#include <Foundation/Types/Bitflags.h>

struct nsMemoryTrackingFlags
{
  using StorageType = nsUInt32;

  enum Enum
  {
    None,
    RegisterAllocator = NS_BIT(0),        ///< Register the allocator with the memory tracker. If EnableAllocationTracking is not set as well it is up to the
                                          ///< allocator implementation whether it collects usable stats or not.
    EnableAllocationTracking = NS_BIT(1), ///< Enable tracking of individual allocations
    EnableStackTrace = NS_BIT(2),         ///< Enable stack traces for each allocation

    All = RegisterAllocator | EnableAllocationTracking | EnableStackTrace,

    Default = 0
#if NS_ENABLED(NS_USE_ALLOCATION_TRACKING)
              | RegisterAllocator | EnableAllocationTracking
#endif
#if NS_ENABLED(NS_USE_ALLOCATION_STACK_TRACING)
              | EnableStackTrace
#endif
  };

  struct Bits
  {
    StorageType RegisterAllocator : 1;
    StorageType EnableAllocationTracking : 1;
    StorageType EnableStackTrace : 1;
  };
};

// NS_DECLARE_FLAGS_OPERATORS(nsMemoryTrackingFlags);

#define NS_STATIC_ALLOCATOR_NAME "Statics"

/// \brief Memory tracker which keeps track of all allocations and constructions
class NS_FOUNDATION_DLL nsMemoryTracker
{
public:
  struct AllocationInfo
  {
    NS_DECLARE_POD_TYPE();

    NS_FORCE_INLINE AllocationInfo()

      = default;

    void** m_pStackTrace = nullptr;
    size_t m_uiSize = 0;
    nsUInt16 m_uiAlignment = 0;
    nsUInt16 m_uiStackTraceLength = 0;

    NS_ALWAYS_INLINE const nsArrayPtr<void*> GetStackTrace() const { return nsArrayPtr<void*>(m_pStackTrace, (nsUInt32)m_uiStackTraceLength); }

    NS_ALWAYS_INLINE nsArrayPtr<void*> GetStackTrace() { return nsArrayPtr<void*>(m_pStackTrace, (nsUInt32)m_uiStackTraceLength); }

    NS_FORCE_INLINE void SetStackTrace(nsArrayPtr<void*> stackTrace)
    {
      m_pStackTrace = stackTrace.GetPtr();
      NS_ASSERT_DEV(stackTrace.GetCount() < 0xFFFF, "stack trace too long");
      m_uiStackTraceLength = (nsUInt16)stackTrace.GetCount();
    }
  };

  class NS_FOUNDATION_DLL Iterator
  {
  public:
    ~Iterator();

    nsAllocatorId Id() const;
    nsStringView Name() const;
    nsAllocatorId ParentId() const;
    const nsAllocatorBase::Stats& Stats() const;

    void Next();
    bool IsValid() const;

    NS_ALWAYS_INLINE void operator++() { Next(); }

  private:
    friend class nsMemoryTracker;

    NS_ALWAYS_INLINE Iterator(void* pData)
      : m_pData(pData)
    {
    }

    void* m_pData;
  };

  static nsAllocatorId RegisterAllocator(nsStringView sName, nsBitflags<nsMemoryTrackingFlags> flags, nsAllocatorId parentId);
  static void DeregisterAllocator(nsAllocatorId allocatorId);

  static void AddAllocation(
    nsAllocatorId allocatorId, nsBitflags<nsMemoryTrackingFlags> flags, const void* pPtr, size_t uiSize, size_t uiAlign, nsTime allocationTime);
  static void RemoveAllocation(nsAllocatorId allocatorId, const void* pPtr);
  static void RemoveAllAllocations(nsAllocatorId allocatorId);
  static void SetAllocatorStats(nsAllocatorId allocatorId, const nsAllocatorBase::Stats& stats);

  static void ResetPerFrameAllocatorStats();

  static nsStringView GetAllocatorName(nsAllocatorId allocatorId);
  static const nsAllocatorBase::Stats& GetAllocatorStats(nsAllocatorId allocatorId);
  static nsAllocatorId GetAllocatorParentId(nsAllocatorId allocatorId);
  static const AllocationInfo& GetAllocationInfo(nsAllocatorId allocatorId, const void* pPtr);

  static void DumpMemoryLeaks();

  static Iterator GetIterator();
};
