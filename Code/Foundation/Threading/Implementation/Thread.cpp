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
