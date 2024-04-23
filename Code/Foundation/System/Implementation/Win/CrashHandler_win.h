/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

#include <Foundation/System/StackTracer.h>

static LONG WINAPI nsCrashHandlerFunc(struct _EXCEPTION_POINTERS* pExceptionInfo)
{
  static nsMutex s_CrashMutex;
  NS_LOCK(s_CrashMutex);

  static bool s_bAlreadyHandled = false;

  if (s_bAlreadyHandled == false)
  {
    if (nsCrashHandler::GetCrashHandler() != nullptr)
    {
      s_bAlreadyHandled = true;
      nsCrashHandler::GetCrashHandler()->HandleCrash(pExceptionInfo);
    }
  }

  return EXCEPTION_CONTINUE_SEARCH;
}

void nsCrashHandler::SetCrashHandler(nsCrashHandler* pHandler)
{
  s_pActiveHandler = pHandler;

  if (s_pActiveHandler != nullptr)
  {
    SetUnhandledExceptionFilter(nsCrashHandlerFunc);
  }
  else
  {
    SetUnhandledExceptionFilter(nullptr);
  }
}

bool nsCrashHandler_WriteMiniDump::WriteOwnProcessMiniDump(void* pOsSpecificData)
{
#if NS_ENABLED(NS_PLATFORM_WINDOWS_DESKTOP)
  nsStatus res = nsMiniDumpUtils::WriteOwnProcessMiniDump(m_sDumpFilePath, (_EXCEPTION_POINTERS*)pOsSpecificData);
  if (res.Failed())
    nsLog::Printf("WriteOwnProcessMiniDump failed: %s\n", res.m_sMessage.GetData());
  return res.Succeeded();
#else
  return false;
#endif
}

void nsCrashHandler_WriteMiniDump::PrintStackTrace(void* pOsSpecificData)
{
  _EXCEPTION_POINTERS* pExceptionInfo = (_EXCEPTION_POINTERS*)pOsSpecificData;

  nsLog::Printf("***Unhandled Exception:***\n");
  nsLog::Printf("Exception: %08x", (nsUInt32)pExceptionInfo->ExceptionRecord->ExceptionCode);

  {
    nsLog::Printf("\n\n***Stack Trace:***\n");
    void* pBuffer[64];
    nsArrayPtr<void*> tempTrace(pBuffer);
    const nsUInt32 uiNumTraces = nsStackTracer::GetStackTrace(tempTrace, pExceptionInfo->ContextRecord);

    nsStackTracer::ResolveStackTrace(tempTrace.GetSubArray(0, uiNumTraces), &PrintHelper);
  }
}
