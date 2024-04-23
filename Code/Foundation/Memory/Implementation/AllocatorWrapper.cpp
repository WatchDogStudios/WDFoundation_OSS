/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Memory/AllocatorWrapper.h>

static thread_local nsAllocatorBase* s_pAllocator = nullptr;

nsLocalAllocatorWrapper::nsLocalAllocatorWrapper(nsAllocatorBase* pAllocator)
{
  s_pAllocator = pAllocator;
}

void nsLocalAllocatorWrapper::Reset()
{
  s_pAllocator = nullptr;
}

nsAllocatorBase* nsLocalAllocatorWrapper::GetAllocator()
{
  return s_pAllocator;
}

NS_STATICLINK_FILE(Foundation, Foundation_Memory_Implementation_AllocatorWrapper);
