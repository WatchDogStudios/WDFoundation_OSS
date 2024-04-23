/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

// Posix implementation of thread helper functions

#include <pthread.h>

static pthread_t g_MainThread = (pthread_t)0;

void nsThreadUtils::Initialize()
{
  g_MainThread = pthread_self();
}

void nsThreadUtils::YieldTimeSlice()
{
  sched_yield();
}

void nsThreadUtils::YieldHardwareThread()
{
  // No equivalent to mm_pause on linux
}

void nsThreadUtils::Sleep(const nsTime& duration)
{
  timespec SleepTime;
  SleepTime.tv_sec = duration.GetSeconds();
  SleepTime.tv_nsec = ((nsInt64)duration.GetMilliseconds() * 1000000LL) % 1000000000LL;
  nanosleep(&SleepTime, nullptr);
}

// nsThreadHandle nsThreadUtils::GetCurrentThreadHandle()
//{
//  return pthread_self();
//}

nsThreadID nsThreadUtils::GetCurrentThreadID()
{
  return pthread_self();
}

bool nsThreadUtils::IsMainThread()
{
  return pthread_self() == g_MainThread;
}
