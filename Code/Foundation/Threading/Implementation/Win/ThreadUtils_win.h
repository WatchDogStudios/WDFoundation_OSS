/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

// Windows implementation of thread helper functions

static DWORD g_uiMainThreadID = 0xFFFFFFFF;

void nsThreadUtils::Initialize()
{
  g_uiMainThreadID = GetCurrentThreadId();
}

void nsThreadUtils::YieldTimeSlice()
{
  ::Sleep(0);
}

void nsThreadUtils::YieldHardwareThread()
{
  YieldProcessor();
}

void nsThreadUtils::Sleep(const nsTime& duration)
{
  ::Sleep((DWORD)duration.GetMilliseconds());
}

nsThreadID nsThreadUtils::GetCurrentThreadID()
{
  return ::GetCurrentThreadId();
}

bool nsThreadUtils::IsMainThread()
{
  return GetCurrentThreadID() == g_uiMainThreadID;
}
