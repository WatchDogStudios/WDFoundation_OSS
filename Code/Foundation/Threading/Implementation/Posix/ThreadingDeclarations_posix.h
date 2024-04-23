/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

// Deactivate Doxygen document generation for the following block.
/// \cond

#include <pthread.h>
#include <semaphore.h>

using nsThreadHandle = pthread_t;
using nsThreadID = pthread_t;
using nsMutexHandle = pthread_mutex_t;
using nsOSThreadEntryPoint = void* (*)(void* pThreadParameter);

struct nsSemaphoreHandle
{
  sem_t* m_pNamedOrUnnamed = nullptr;
  sem_t* m_pNamed = nullptr;
  sem_t m_Unnamed;
};

#define NS_THREAD_CLASS_ENTRY_POINT void* nsThreadClassEntryPoint(void* pThreadParameter);

struct nsConditionVariableData
{
  pthread_cond_t m_ConditionVariable;
};


/// \endcond
