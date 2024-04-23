/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

nsAtomicInteger32 nsOSThread::s_iThreadCount;

// Posix specific implementation of the thread class

nsOSThread::nsOSThread(
  nsOSThreadEntryPoint pThreadEntryPoint, void* pUserData /*= nullptr*/, const char* szName /*= "nsThread"*/, nsUInt32 uiStackSize /*= 128 * 1024*/)
{
  s_iThreadCount.Increment();

  m_EntryPoint = pThreadEntryPoint;
  m_pUserData = pUserData;
  m_szName = szName;
  m_uiStackSize = uiStackSize;

  // Thread creation is deferred since Posix threads can't be created sleeping
}

nsOSThread::~nsOSThread()
{
  s_iThreadCount.Decrement();
}

/// Starts the thread
void nsOSThread::Start()
{
  pthread_attr_t ThreadAttributes;
  pthread_attr_init(&ThreadAttributes);
  pthread_attr_setdetachstate(&ThreadAttributes, PTHREAD_CREATE_JOINABLE);
  pthread_attr_setstacksize(&ThreadAttributes, m_uiStackSize);

  int iReturnCode = pthread_create(&m_hHandle, &ThreadAttributes, m_EntryPoint, m_pUserData);
  NS_IGNORE_UNUSED(iReturnCode);
  NS_ASSERT_RELEASE(iReturnCode == 0, "Thread creation failed!");

#if NS_ENABLED(NS_PLATFORM_LINUX) || NS_ENABLED(NS_PLATFORM_ANDROID)
  if (iReturnCode == 0 && m_szName != nullptr)
  {
    // pthread has a thread name limit of 16 bytes.
    // This means 15 characters and the terminating '\0'
    if (strlen(m_szName) < 16)
    {
      pthread_setname_np(m_hHandle, m_szName);
    }
    else
    {
      char threadName[16];
      strncpy(threadName, m_szName, 15);
      threadName[15] = '\0';
      pthread_setname_np(m_hHandle, threadName);
    }
  }
#endif

  m_ThreadID = m_hHandle;

  pthread_attr_destroy(&ThreadAttributes);
}

/// Joins with the thread (waits for termination)
void nsOSThread::Join()
{
  pthread_join(m_hHandle, nullptr);
}
