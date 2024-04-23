/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#ifdef NS_STACKTRACER_UWP_INL_H_INCLUDED
#  error "This file must not be included twice."
#endif

#define NS_STACKTRACER_UWP_INL_H_INCLUDED

#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

void nsStackTracer::OnPluginEvent(const nsPluginEvent& e) {}

// static
nsUInt32 nsStackTracer::GetStackTrace(nsArrayPtr<void*>& trace, void* pContext)
{
  return 0;
}

// static
void nsStackTracer::ResolveStackTrace(const nsArrayPtr<void*>& trace, PrintFunc printFunc)
{
  char szBuffer[512] = "Stack Traces are currently not supported on UWP";

  printFunc(szBuffer);
}
