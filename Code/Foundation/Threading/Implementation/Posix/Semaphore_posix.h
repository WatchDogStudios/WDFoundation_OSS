/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

// THIS IMPLEMENTATION IS UNTESTED (and may not even compile)

#include <Foundation/Strings/StringBuilder.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

nsSemaphore::nsSemaphore() = default;

nsSemaphore::~nsSemaphore()
{
  if (m_hSemaphore.m_pNamedOrUnnamed != nullptr)
  {
    if (m_hSemaphore.m_pNamed != nullptr)
    {
      sem_close(m_hSemaphore.m_pNamed);
    }
    else
    {
      sem_destroy(&m_hSemaphore.m_Unnamed);
    }
  }
}

nsResult nsSemaphore::Create(nsUInt32 uiInitialTokenCount, nsStringView sSharedName /*= nullptr*/)
{
  if (sSharedName.IsEmpty())
  {
    // create an unnamed semaphore

    if (sem_init(&m_hSemaphore.m_Unnamed, 0, uiInitialTokenCount) != 0)
    {
      return NS_FAILURE;
    }

    m_hSemaphore.m_pNamedOrUnnamed = &m_hSemaphore.m_Unnamed;
  }
  else
  {
    // create a named semaphore

    // documentation is unclear about access rights, just throwing everything at it for good measure
    nsStringBuilder tmp;
    m_hSemaphore.m_pNamed = sem_open(sSharedName.GetData(tmp), O_CREAT | O_EXCL, S_IRWXU | S_IRWXO | S_IRWXG, uiInitialTokenCount);

    if (m_hSemaphore.m_pNamed == nullptr)
    {
      return NS_FAILURE;
    }

    m_hSemaphore.m_pNamedOrUnnamed = m_hSemaphore.m_pNamed;
  }

  return NS_SUCCESS;
}

nsResult nsSemaphore::Open(nsStringView sSharedName)
{
  NS_ASSERT_DEV(!sSharedName.IsEmpty(), "Name of semaphore to open mustn't be empty.");

  // open a named semaphore

  nsStringBuilder tmp;
  m_hSemaphore.m_pNamed = sem_open(sSharedName.GetData(tmp), 0);

  if (m_hSemaphore.m_pNamed == nullptr)
  {
    return NS_FAILURE;
  }

  m_hSemaphore.m_pNamedOrUnnamed = m_hSemaphore.m_pNamed;

  return NS_SUCCESS;
}

void nsSemaphore::AcquireToken()
{
  NS_VERIFY(sem_wait(m_hSemaphore.m_pNamedOrUnnamed) == 0, "Semaphore token acquisition failed.");
}

void nsSemaphore::ReturnToken()
{
  NS_VERIFY(sem_post(m_hSemaphore.m_pNamedOrUnnamed) == 0, "Returning a semaphore token failed, most likely due to a AcquireToken() / ReturnToken() mismatch.");
}

nsResult nsSemaphore::TryAcquireToken()
{
  // documentation is unclear whether one needs to check errno, or not
  // assuming that this will return 0 only when trywait got a token

  if (sem_trywait(m_hSemaphore.m_pNamedOrUnnamed) == 0)
    return NS_SUCCESS;

  return NS_FAILURE;
}
