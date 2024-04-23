/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Containers/HashTable.h>
#include <Foundation/Containers/IdTable.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Memory/Allocator.h>
#include <Foundation/Memory/Policies/HeapAllocation.h>
#include <Foundation/Strings/String.h>
#include <Foundation/System/StackTracer.h>
#include <Foundation/Threading/Lock.h>
#include <Foundation/Threading/Mutex.h>

#if NS_ENABLED(NS_PLATFORM_WINDOWS)
#  include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#endif

namespace
{
  // no tracking for the tracker data itself
  using TrackerDataAllocator = nsAllocator<nsMemoryPolicies::nsHeapAllocation, 0>;

  static TrackerDataAllocator* s_pTrackerDataAllocator;

  struct TrackerDataAllocatorWrapper
  {
    NS_ALWAYS_INLINE static nsAllocatorBase* GetAllocator() { return s_pTrackerDataAllocator; }
  };


  struct AllocatorData
  {
    NS_ALWAYS_INLINE AllocatorData() = default;

    nsHybridString<32, TrackerDataAllocatorWrapper> m_sName;
    nsBitflags<nsMemoryTrackingFlags> m_Flags;

    nsAllocatorId m_ParentId;

    nsAllocatorBase::Stats m_Stats;

    nsHashTable<const void*, nsMemoryTracker::AllocationInfo, nsHashHelper<const void*>, TrackerDataAllocatorWrapper> m_Allocations;
  };

  struct TrackerData
  {
    NS_ALWAYS_INLINE void Lock() { m_Mutex.Lock(); }
    NS_ALWAYS_INLINE void Unlock() { m_Mutex.Unlock(); }

    nsMutex m_Mutex;

    using AllocatorTable = nsIdTable<nsAllocatorId, AllocatorData, TrackerDataAllocatorWrapper>;
    AllocatorTable m_AllocatorData;

    nsAllocatorId m_StaticAllocatorId;
  };

  static TrackerData* s_pTrackerData;
  static bool s_bIsInitialized = false;
  static bool s_bIsInitializing = false;

  static void Initialize()
  {
    if (s_bIsInitialized)
      return;

    NS_ASSERT_DEV(!s_bIsInitializing, "MemoryTracker initialization entered recursively");
    s_bIsInitializing = true;

    if (s_pTrackerDataAllocator == nullptr)
    {
      alignas(NS_ALIGNMENT_OF(TrackerDataAllocator)) static nsUInt8 TrackerDataAllocatorBuffer[sizeof(TrackerDataAllocator)];
      s_pTrackerDataAllocator = new (TrackerDataAllocatorBuffer) TrackerDataAllocator("MemoryTracker");
      NS_ASSERT_DEV(s_pTrackerDataAllocator != nullptr, "MemoryTracker initialization failed");
    }

    if (s_pTrackerData == nullptr)
    {
      alignas(NS_ALIGNMENT_OF(TrackerData)) static nsUInt8 TrackerDataBuffer[sizeof(TrackerData)];
      s_pTrackerData = new (TrackerDataBuffer) TrackerData();
      NS_ASSERT_DEV(s_pTrackerData != nullptr, "MemoryTracker initialization failed");
    }

    s_bIsInitialized = true;
    s_bIsInitializing = false;
  }

  static void DumpLeak(const nsMemoryTracker::AllocationInfo& info, const char* szAllocatorName)
  {
    char szBuffer[512];
    nsUInt64 uiSize = info.m_uiSize;
    nsStringUtils::snprintf(szBuffer, NS_ARRAY_SIZE(szBuffer), "Leaked %llu bytes allocated by '%s'\n", uiSize, szAllocatorName);

    nsLog::Print(szBuffer);

    if (info.GetStackTrace().GetPtr() != nullptr)
    {
      nsStackTracer::ResolveStackTrace(info.GetStackTrace(), &nsLog::Print);
    }

    nsLog::Print("--------------------------------------------------------------------\n\n");
  }
} // namespace

// Iterator
#define CAST_ITER(ptr) static_cast<TrackerData::AllocatorTable::Iterator*>(ptr)

nsAllocatorId nsMemoryTracker::Iterator::Id() const
{
  return CAST_ITER(m_pData)->Id();
}

nsStringView nsMemoryTracker::Iterator::Name() const
{
  return CAST_ITER(m_pData)->Value().m_sName;
}

nsAllocatorId nsMemoryTracker::Iterator::ParentId() const
{
  return CAST_ITER(m_pData)->Value().m_ParentId;
}

const nsAllocatorBase::Stats& nsMemoryTracker::Iterator::Stats() const
{
  return CAST_ITER(m_pData)->Value().m_Stats;
}

void nsMemoryTracker::Iterator::Next()
{
  CAST_ITER(m_pData)->Next();
}

bool nsMemoryTracker::Iterator::IsValid() const
{
  return CAST_ITER(m_pData)->IsValid();
}

nsMemoryTracker::Iterator::~Iterator()
{
  auto it = CAST_ITER(m_pData);
  NS_DELETE(s_pTrackerDataAllocator, it);
  m_pData = nullptr;
}


// static
nsAllocatorId nsMemoryTracker::RegisterAllocator(nsStringView sName, nsBitflags<nsMemoryTrackingFlags> flags, nsAllocatorId parentId)
{
  Initialize();

  NS_LOCK(*s_pTrackerData);

  AllocatorData data;
  data.m_sName = sName;
  data.m_Flags = flags;
  data.m_ParentId = parentId;

  nsAllocatorId id = s_pTrackerData->m_AllocatorData.Insert(data);

  if (data.m_sName == NS_STATIC_ALLOCATOR_NAME)
  {
    s_pTrackerData->m_StaticAllocatorId = id;
  }

  return id;
}

// static
void nsMemoryTracker::DeregisterAllocator(nsAllocatorId allocatorId)
{
  NS_LOCK(*s_pTrackerData);

  const AllocatorData& data = s_pTrackerData->m_AllocatorData[allocatorId];

  nsUInt32 uiLiveAllocations = data.m_Allocations.GetCount();
  if (uiLiveAllocations != 0)
  {
    for (auto it = data.m_Allocations.GetIterator(); it.IsValid(); ++it)
    {
      DumpLeak(it.Value(), data.m_sName.GetData());
    }

    NS_REPORT_FAILURE("Allocator '{0}' leaked {1} allocation(s)", data.m_sName.GetData(), uiLiveAllocations);
  }

  s_pTrackerData->m_AllocatorData.Remove(allocatorId);
}

// static
void nsMemoryTracker::AddAllocation(nsAllocatorId allocatorId, nsBitflags<nsMemoryTrackingFlags> flags, const void* pPtr, size_t uiSize, size_t uiAlign, nsTime allocationTime)
{
  NS_ASSERT_DEV((flags & nsMemoryTrackingFlags::EnableAllocationTracking) != 0, "Allocation tracking is turned off, but nsMemoryTracker::AddAllocation() is called anyway.");

  NS_ASSERT_DEV(uiAlign < 0xFFFF, "Alignment too big");

  nsArrayPtr<void*> stackTrace;
  if (flags.IsSet(nsMemoryTrackingFlags::EnableStackTrace))
  {
    void* pBuffer[64];
    nsArrayPtr<void*> tempTrace(pBuffer);
    const nsUInt32 uiNumTraces = nsStackTracer::GetStackTrace(tempTrace);

    stackTrace = NS_NEW_ARRAY(s_pTrackerDataAllocator, void*, uiNumTraces);
    nsMemoryUtils::Copy(stackTrace.GetPtr(), pBuffer, uiNumTraces);
  }

  {
    NS_LOCK(*s_pTrackerData);

    AllocatorData& data = s_pTrackerData->m_AllocatorData[allocatorId];
    data.m_Stats.m_uiNumAllocations++;
    data.m_Stats.m_uiAllocationSize += uiSize;
    data.m_Stats.m_uiPerFrameAllocationSize += uiSize;
    data.m_Stats.m_PerFrameAllocationTime += allocationTime;

    NS_ASSERT_DEBUG(data.m_Flags == flags, "Given flags have to be identical to allocator flags");
    auto pInfo = &data.m_Allocations[pPtr];
    pInfo->m_uiSize = uiSize;
    pInfo->m_uiAlignment = (nsUInt16)uiAlign;
    pInfo->SetStackTrace(stackTrace);
  }
}

// static
void nsMemoryTracker::RemoveAllocation(nsAllocatorId allocatorId, const void* pPtr)
{
  nsArrayPtr<void*> stackTrace;

  {
    NS_LOCK(*s_pTrackerData);

    AllocatorData& data = s_pTrackerData->m_AllocatorData[allocatorId];

    AllocationInfo info;
    if (data.m_Allocations.Remove(pPtr, &info))
    {
      data.m_Stats.m_uiNumDeallocations++;
      data.m_Stats.m_uiAllocationSize -= info.m_uiSize;

      stackTrace = info.GetStackTrace();
    }
    else
    {
      NS_REPORT_FAILURE("Invalid Allocation '{0}'. Memory corruption?", nsArgP(pPtr));
    }
  }

  NS_DELETE_ARRAY(s_pTrackerDataAllocator, stackTrace);
}

// static
void nsMemoryTracker::RemoveAllAllocations(nsAllocatorId allocatorId)
{
  NS_LOCK(*s_pTrackerData);
  AllocatorData& data = s_pTrackerData->m_AllocatorData[allocatorId];
  for (auto it = data.m_Allocations.GetIterator(); it.IsValid(); ++it)
  {
    auto& info = it.Value();
    data.m_Stats.m_uiNumDeallocations++;
    data.m_Stats.m_uiAllocationSize -= info.m_uiSize;

    NS_DELETE_ARRAY(s_pTrackerDataAllocator, info.GetStackTrace());
  }
  data.m_Allocations.Clear();
}

// static
void nsMemoryTracker::SetAllocatorStats(nsAllocatorId allocatorId, const nsAllocatorBase::Stats& stats)
{
  NS_LOCK(*s_pTrackerData);

  s_pTrackerData->m_AllocatorData[allocatorId].m_Stats = stats;
}

// static
void nsMemoryTracker::ResetPerFrameAllocatorStats()
{
  NS_LOCK(*s_pTrackerData);

  for (auto it = s_pTrackerData->m_AllocatorData.GetIterator(); it.IsValid(); ++it)
  {
    AllocatorData& data = it.Value();
    data.m_Stats.m_uiPerFrameAllocationSize = 0;
    data.m_Stats.m_PerFrameAllocationTime = nsTime::MakeZero();
  }
}

// static
nsStringView nsMemoryTracker::GetAllocatorName(nsAllocatorId allocatorId)
{
  NS_LOCK(*s_pTrackerData);

  return s_pTrackerData->m_AllocatorData[allocatorId].m_sName;
}

// static
const nsAllocatorBase::Stats& nsMemoryTracker::GetAllocatorStats(nsAllocatorId allocatorId)
{
  NS_LOCK(*s_pTrackerData);

  return s_pTrackerData->m_AllocatorData[allocatorId].m_Stats;
}

// static
nsAllocatorId nsMemoryTracker::GetAllocatorParentId(nsAllocatorId allocatorId)
{
  NS_LOCK(*s_pTrackerData);

  return s_pTrackerData->m_AllocatorData[allocatorId].m_ParentId;
}

// static
const nsMemoryTracker::AllocationInfo& nsMemoryTracker::GetAllocationInfo(nsAllocatorId allocatorId, const void* pPtr)
{
  NS_LOCK(*s_pTrackerData);

  const AllocatorData& data = s_pTrackerData->m_AllocatorData[allocatorId];
  const AllocationInfo* info = nullptr;
  if (data.m_Allocations.TryGetValue(pPtr, info))
  {
    return *info;
  }

  static AllocationInfo invalidInfo;

  NS_REPORT_FAILURE("Could not find info for allocation {0}", nsArgP(pPtr));
  return invalidInfo;
}


struct LeakInfo
{
  NS_DECLARE_POD_TYPE();

  nsAllocatorId m_AllocatorId;
  size_t m_uiSize = 0;
  const void* m_pParentLeak = nullptr;

  NS_ALWAYS_INLINE bool IsRootLeak() const { return m_pParentLeak == nullptr && m_AllocatorId != s_pTrackerData->m_StaticAllocatorId; }
};

// static
void nsMemoryTracker::DumpMemoryLeaks()
{
  if (s_pTrackerData == nullptr) // if both tracking and tracing is disabled there is no tracker data
    return;
  NS_LOCK(*s_pTrackerData);

  static nsHashTable<const void*, LeakInfo, nsHashHelper<const void*>, TrackerDataAllocatorWrapper> leakTable;
  leakTable.Clear();

  // first collect all leaks
  for (auto it = s_pTrackerData->m_AllocatorData.GetIterator(); it.IsValid(); ++it)
  {
    const AllocatorData& data = it.Value();
    for (auto it2 = data.m_Allocations.GetIterator(); it2.IsValid(); ++it2)
    {
      LeakInfo leak;
      leak.m_AllocatorId = it.Id();
      leak.m_uiSize = it2.Value().m_uiSize;
      leak.m_pParentLeak = nullptr;

      leakTable.Insert(it2.Key(), leak);
    }
  }

  // find dependencies
  for (auto it = leakTable.GetIterator(); it.IsValid(); ++it)
  {
    const void* ptr = it.Key();
    const LeakInfo& leak = it.Value();

    const void* curPtr = ptr;
    const void* endPtr = nsMemoryUtils::AddByteOffset(ptr, leak.m_uiSize);

    while (curPtr < endPtr)
    {
      const void* testPtr = *reinterpret_cast<const void* const*>(curPtr);

      LeakInfo* dependentLeak = nullptr;
      if (leakTable.TryGetValue(testPtr, dependentLeak))
      {
        dependentLeak->m_pParentLeak = ptr;
      }

      curPtr = nsMemoryUtils::AddByteOffset(curPtr, sizeof(void*));
    }
  }

  // dump leaks
  nsUInt64 uiNumLeaks = 0;

  for (auto it = leakTable.GetIterator(); it.IsValid(); ++it)
  {
    const void* ptr = it.Key();
    const LeakInfo& leak = it.Value();

    if (leak.IsRootLeak())
    {
      if (uiNumLeaks == 0)
      {
        nsLog::Print("\n\n--------------------------------------------------------------------\n"
                     "Memory Leak Report:"
                     "\n--------------------------------------------------------------------\n\n");
      }

      const AllocatorData& data = s_pTrackerData->m_AllocatorData[leak.m_AllocatorId];
      nsMemoryTracker::AllocationInfo info;
      data.m_Allocations.TryGetValue(ptr, info);

      DumpLeak(info, data.m_sName.GetData());

      ++uiNumLeaks;
    }
  }

  if (uiNumLeaks > 0)
  {
    nsLog::Printf("\n--------------------------------------------------------------------\n"
                  "Found %llu root memory leak(s)."
                  "\n--------------------------------------------------------------------\n\n",
      uiNumLeaks);

    NS_REPORT_FAILURE("Found {0} root memory leak(s).", uiNumLeaks);
  }
}

// static
nsMemoryTracker::Iterator nsMemoryTracker::GetIterator()
{
  auto pInnerIt = NS_NEW(s_pTrackerDataAllocator, TrackerData::AllocatorTable::Iterator, s_pTrackerData->m_AllocatorData.GetIterator());
  return Iterator(pInnerIt);
}


NS_STATICLINK_FILE(Foundation, Foundation_Memory_Implementation_MemoryTracker);
