/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Threading/ConditionVariable.h>

void nsConditionVariable::Lock()
{
  m_Mutex.Lock();
  ++m_iLockCount;
}

nsResult nsConditionVariable::TryLock()
{
  if (m_Mutex.TryLock().Succeeded())
  {
    ++m_iLockCount;
    return NS_SUCCESS;
  }

  return NS_FAILURE;
}

void nsConditionVariable::Unlock()
{
  NS_ASSERT_DEV(m_iLockCount > 0, "Cannot unlock a thread-signal that was not locked before.");
  --m_iLockCount;
  m_Mutex.Unlock();
}

#if NS_ENABLED(NS_PLATFORM_WINDOWS)
#  include <Foundation/Threading/Implementation/Win/ConditionVariable_win.h>
#elif NS_ENABLED(NS_PLATFORM_OSX) || NS_ENABLED(NS_PLATFORM_LINUX) || NS_ENABLED(NS_PLATFORM_ANDROID)
#  include <Foundation/Threading/Implementation/Posix/ConditionVariable_posix.h>
#else
#  error "Unsupported Platform."
#endif



NS_STATICLINK_FILE(Foundation, Foundation_Threading_Implementation_ConditionVariable);
