/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

struct nsNullAllocatorWrapper
{
  NS_FORCE_INLINE static nsAllocatorBase* GetAllocator()
  {
    NS_REPORT_FAILURE("This method should never be called");
    return nullptr;
  }
};

struct nsDefaultAllocatorWrapper
{
  NS_ALWAYS_INLINE static nsAllocatorBase* GetAllocator() { return nsFoundation::GetDefaultAllocator(); }
};

struct nsStaticAllocatorWrapper
{
  NS_ALWAYS_INLINE static nsAllocatorBase* GetAllocator() { return nsFoundation::GetStaticAllocator(); }
};

struct nsAlignedAllocatorWrapper
{
  NS_ALWAYS_INLINE static nsAllocatorBase* GetAllocator() { return nsFoundation::GetAlignedAllocator(); }
};

struct NS_FOUNDATION_DLL nsLocalAllocatorWrapper
{
  nsLocalAllocatorWrapper(nsAllocatorBase* pAllocator);

  void Reset();

  static nsAllocatorBase* GetAllocator();
};
