/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#ifdef NS_STACKTRACER_POSIX_INL_H_INCLUDED
#  error "This file must not be included twice."
#endif

#define NS_STACKTRACER_POSIX_INL_H_INCLUDED

#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

#include <Foundation/Math/Math.h>
#include <execinfo.h>

void nsStackTracer::OnPluginEvent(const nsPluginEvent& e) {}

// static
nsUInt32 nsStackTracer::GetStackTrace(nsArrayPtr<void*>& trace, void* pContext)
{
  int iSymbols = backtrace(trace.GetPtr(), trace.GetCount());

  return iSymbols;
}

// static
void nsStackTracer::ResolveStackTrace(const nsArrayPtr<void*>& trace, PrintFunc printFunc)
{
  char szBuffer[512];

  char** ppSymbols = backtrace_symbols(trace.GetPtr(), trace.GetCount());

  if (ppSymbols != nullptr)
  {
    for (nsUInt32 i = 0; i < trace.GetCount(); i++)
    {
      int iLen = nsMath::Min(strlen(ppSymbols[i]), (size_t)NS_ARRAY_SIZE(szBuffer) - 2);
      memcpy(szBuffer, ppSymbols[i], iLen);
      szBuffer[iLen] = '\n';
      szBuffer[iLen + 1] = '\0';

      printFunc(szBuffer);
    }

    free(ppSymbols);
  }
}
