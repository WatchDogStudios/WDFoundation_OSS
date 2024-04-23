/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

#include <errno.h>
#include <pthread.h>

nsConditionVariable::nsConditionVariable()
{
  pthread_cond_init(&m_Data.m_ConditionVariable, nullptr);
}

nsConditionVariable::~nsConditionVariable()
{
  NS_ASSERT_DEV(m_iLockCount == 0, "Thread-signal must be unlocked during destruction.");

  pthread_cond_destroy(&m_Data.m_ConditionVariable);
}

void nsConditionVariable::SignalOne()
{
  pthread_cond_signal(&m_Data.m_ConditionVariable);
}

void nsConditionVariable::SignalAll()
{
  pthread_cond_broadcast(&m_Data.m_ConditionVariable);
}

void nsConditionVariable::UnlockWaitForSignalAndLock() const
{
  NS_ASSERT_DEV(m_iLockCount > 0, "nsConditionVariable must be locked when calling UnlockWaitForSignalAndLock.");

  pthread_cond_wait(&m_Data.m_ConditionVariable, &m_Mutex.GetMutexHandle());
}

nsConditionVariable::WaitResult nsConditionVariable::UnlockWaitForSignalAndLock(nsTime timeout) const
{
  NS_ASSERT_DEV(m_iLockCount > 0, "nsConditionVariable must be locked when calling UnlockWaitForSignalAndLock.");

  // inside the lock
  --m_iLockCount;

  timeval now;
  gettimeofday(&now, nullptr);

  // pthread_cond_timedwait needs an absolute time value, so compute it from the current time.
  struct timespec timeToWait;

  const nsInt64 iNanoSecondsPerSecond = 1000000000LL;
  const nsInt64 iMicroSecondsPerNanoSecond = 1000LL;

  nsInt64 endTime = now.tv_sec * iNanoSecondsPerSecond + now.tv_usec * iMicroSecondsPerNanoSecond + static_cast<nsInt64>(timeout.GetNanoseconds());

  timeToWait.tv_sec = endTime / iNanoSecondsPerSecond;
  timeToWait.tv_nsec = endTime % iNanoSecondsPerSecond;

  if (pthread_cond_timedwait(&m_Data.m_ConditionVariable, &m_Mutex.GetMutexHandle(), &timeToWait) == ETIMEDOUT)
  {
    // inside the lock
    ++m_iLockCount;
    return WaitResult::Timeout;
  }

  // inside the lock
  ++m_iLockCount;
  return WaitResult::Signaled;
}
