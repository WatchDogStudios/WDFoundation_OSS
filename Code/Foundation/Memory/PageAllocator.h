/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Basics.h>

/// \brief This helper class can reserve and allocate whole memory pages.
class NS_FOUNDATION_DLL nsPageAllocator
{
public:
  static void* AllocatePage(size_t uiSize);
  static void DeallocatePage(void* pPtr);

  static nsAllocatorId GetId();
};
