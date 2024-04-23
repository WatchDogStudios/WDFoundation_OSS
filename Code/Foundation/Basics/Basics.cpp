/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Memory/CommonAllocators.h>

#if NS_ENABLED(NS_USE_GUARDED_ALLOCATIONS)
using DefaultHeapType = nsGuardedAllocator;
using DefaultAlignedHeapType = nsGuardedAllocator;
using DefaultStaticHeapType = nsGuardedAllocator;
#else
using DefaultHeapType = nsHeapAllocator;
using DefaultAlignedHeapType = nsAlignedHeapAllocator;
using DefaultStaticHeapType = nsHeapAllocator;
#endif

enum
{
  HEAP_ALLOCATOR_BUFFER_SIZE = sizeof(DefaultHeapType),
  ALIGNED_ALLOCATOR_BUFFER_SIZE = sizeof(DefaultAlignedHeapType)
};

alignas(NS_ALIGNMENT_MINIMUM) static nsUInt8 s_DefaultAllocatorBuffer[HEAP_ALLOCATOR_BUFFER_SIZE];
alignas(NS_ALIGNMENT_MINIMUM) static nsUInt8 s_StaticAllocatorBuffer[HEAP_ALLOCATOR_BUFFER_SIZE];

alignas(NS_ALIGNMENT_MINIMUM) static nsUInt8 s_AlignedAllocatorBuffer[ALIGNED_ALLOCATOR_BUFFER_SIZE];

bool nsFoundation::s_bIsInitialized = false;
nsAllocatorBase* nsFoundation::s_pDefaultAllocator = nullptr;
nsAllocatorBase* nsFoundation::s_pAlignedAllocator = nullptr;

void nsFoundation::Initialize()
{
  if (s_bIsInitialized)
    return;

#if NS_ENABLED(NS_COMPILE_FOR_DEVELOPMENT)
  nsMemoryUtils::ReserveLower4GBAddressSpace();
#endif

  if (s_pDefaultAllocator == nullptr)
  {
    s_pDefaultAllocator = new (s_DefaultAllocatorBuffer) DefaultHeapType("DefaultHeap");
  }

  if (s_pAlignedAllocator == nullptr)
  {
    s_pAlignedAllocator = new (s_AlignedAllocatorBuffer) DefaultAlignedHeapType("AlignedHeap");
  }

  s_bIsInitialized = true;
}

#if defined(NS_CUSTOM_STATIC_ALLOCATOR_FUNC)
extern nsAllocatorBase* NS_CUSTOM_STATIC_ALLOCATOR_FUNC();
#endif

nsAllocatorBase* nsFoundation::GetStaticAllocator()
{
  static nsAllocatorBase* pStaticAllocator = nullptr;

  if (pStaticAllocator == nullptr)
  {
#if defined(NS_CUSTOM_STATIC_ALLOCATOR_FUNC)

#  if NS_ENABLED(NS_COMPILE_ENGINE_AS_DLL)

#    if NS_ENABLED(NS_PLATFORM_WINDOWS)
    using GetStaticAllocatorFunc = nsAllocatorBase* (*)();

    HMODULE hThisModule = GetModuleHandle(nullptr);
    GetStaticAllocatorFunc func = (GetStaticAllocatorFunc)GetProcAddress(hThisModule, NS_CUSTOM_STATIC_ALLOCATOR_FUNC);
    if (func != nullptr)
    {
      pStaticAllocator = (*func)();
      return pStaticAllocator;
    }
#    else
#      error "Customizing static allocator not implemented"
#    endif

#  else
    return NS_CUSTOM_STATIC_ALLOCATOR_FUNC();
#  endif

#endif

    pStaticAllocator = new (s_StaticAllocatorBuffer) DefaultStaticHeapType(NS_STATIC_ALLOCATOR_NAME);
  }

  return pStaticAllocator;
}



NS_STATICLINK_FILE(Foundation, Foundation_Basics_Basics);
