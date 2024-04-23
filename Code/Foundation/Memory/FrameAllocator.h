/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Memory/StackAllocator.h>

/// \brief A double buffered stack allocator
class NS_FOUNDATION_DLL nsDoubleBufferedStackAllocator
{
public:
  using StackAllocatorType = nsStackAllocator<nsMemoryTrackingFlags::RegisterAllocator>;

  nsDoubleBufferedStackAllocator(nsStringView sName, nsAllocatorBase* pParent);
  ~nsDoubleBufferedStackAllocator();

  NS_ALWAYS_INLINE nsAllocatorBase* GetCurrentAllocator() const { return m_pCurrentAllocator; }

  void Swap();
  void Reset();

private:
  StackAllocatorType* m_pCurrentAllocator;
  StackAllocatorType* m_pOtherAllocator;
};

class NS_FOUNDATION_DLL nsFrameAllocator
{
public:
  NS_ALWAYS_INLINE static nsAllocatorBase* GetCurrentAllocator() { return s_pAllocator->GetCurrentAllocator(); }

  static void Swap();
  static void Reset();

private:
  NS_MAKE_SUBSYSTEM_STARTUP_FRIEND(Foundation, FrameAllocator);

  static void Startup();
  static void Shutdown();

  static nsDoubleBufferedStackAllocator* s_pAllocator;
};
