/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

// Deactivate Doxygen document generation for the following block.
/// \cond

// Thread entry point used to launch nsRunnable instances
void* nsThreadClassEntryPoint(void* pThreadParameter)
{
  NS_ASSERT_RELEASE(pThreadParameter != nullptr, "thread parameter in thread entry point must not be nullptr!");

  nsThread* pThread = reinterpret_cast<nsThread*>(pThreadParameter);

  RunThread(pThread);

  return nullptr;
}

/// \endcond
