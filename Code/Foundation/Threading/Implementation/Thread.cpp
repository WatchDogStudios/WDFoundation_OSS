/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Profiling/Profiling.h>
#include <Foundation/Threading/Thread.h>

nsEvent<const nsThreadEvent&, nsMutex> nsThread::s_ThreadEvents;

nsThread::nsThread(const char* szName /*= "nsThread"*/, nsUInt32 uiStackSize /*= 128 * 1024*/)
  : nsOSThread(nsThreadClassEntryPoint, this, szName, uiStackSize)
  , m_sName(szName)
{
  nsThreadEvent e;
  e.m_pThread = this;
  e.m_Type = nsThreadEvent::Type::ThreadCreated;
  nsThread::s_ThreadEvents.Broadcast(e, 255);
}

nsThread::~nsThread()
{
  NS_ASSERT_DEV(!IsRunning(), "Thread deletion while still running detected!");

  nsThreadEvent e;
  e.m_pThread = this;
  e.m_Type = nsThreadEvent::Type::ThreadDestroyed;
  nsThread::s_ThreadEvents.Broadcast(e, 255);
}

// Deactivate Doxygen document generation for the following block.
/// \cond

nsUInt32 RunThread(nsThread* pThread)
{
  if (pThread == nullptr)
    return 0;

  nsProfilingSystem::SetThreadName(pThread->m_sName.GetData());

  {
    nsThreadEvent e;
    e.m_pThread = pThread;
    e.m_Type = nsThreadEvent::Type::StartingExecution;
    nsThread::s_ThreadEvents.Broadcast(e, 255);
  }

  pThread->m_ThreadStatus = nsThread::Running;

  // Run the worker thread function
  nsUInt32 uiReturnCode = pThread->Run();

  {
    nsThreadEvent e;
    e.m_pThread = pThread;
    e.m_Type = nsThreadEvent::Type::FinishedExecution;
    nsThread::s_ThreadEvents.Broadcast(e, 255);
  }

  pThread->m_ThreadStatus = nsThread::Finished;

  nsProfilingSystem::RemoveThread();

  return uiReturnCode;
}

/// \endcond

// Include inline file
#if NS_ENABLED(NS_PLATFORM_WINDOWS)
#  include <Foundation/Threading/Implementation/Win/Thread_win.h>
#elif NS_ENABLED(NS_PLATFORM_OSX) || NS_ENABLED(NS_PLATFORM_LINUX) || NS_ENABLED(NS_PLATFORM_ANDROID)
#  include <Foundation/Threading/Implementation/Posix/Thread_posix.h>
#else
#  error "Runnable thread entry functions are not implemented on current platform"
#endif


NS_STATICLINK_FILE(Foundation, Foundation_Threading_Implementation_Thread);
