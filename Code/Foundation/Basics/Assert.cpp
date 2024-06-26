/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#include <Foundation/Logging/Log.h>
#include <Foundation/Strings/StringBuilder.h>
#include <Foundation/Strings/StringUtils.h>
#include <Foundation/System/EnvironmentVariableUtils.h>
#include <Foundation/System/SystemInformation.h>
#include <Foundation/Utilities/ConversionUtils.h>

#include <cstdio>
#include <cstdlib>
#include <ctime>

#if NS_ENABLED(NS_PLATFORM_WINDOWS) && NS_ENABLED(NS_COMPILE_FOR_DEVELOPMENT)
#  include <crtdbg.h>
#endif

#if NS_ENABLED(NS_COMPILER_MSVC)
void MSVC_OutOfLine_DebugBreak(...)
{
  __debugbreak();
}
#endif

bool nsDefaultAssertHandler(const char* szSourceFile, nsUInt32 uiLine, const char* szFunction, const char* szExpression, const char* szAssertMsg)
{
  char szTemp[1024 * 4] = "";
  nsStringUtils::snprintf(szTemp, NS_ARRAY_SIZE(szTemp),
    "\n\n *** Assertion ***\n\n    Expression: \"%s\"\n    Function: \"%s\"\n    File: \"%s\"\n    Line: %u\n    Message: \"%s\"\n\n", szExpression,
    szFunction, szSourceFile, uiLine, szAssertMsg);
  szTemp[1024 * 4 - 1] = '\0';

  nsLog::Print(szTemp);

  if (nsSystemInformation::IsDebuggerAttached())
    return true;

  // If no debugger is attached we append the assert to a common file so that postmortem debugging is easier
  if (FILE* assertLogFP = fopen("nsDefaultAssertHandlerOutput.txt", "a"))
  {
    time_t timeUTC = time(&timeUTC);
    tm* ptm = gmtime(&timeUTC);

    char szTimeStr[256] = {0};
    sprintf(szTimeStr, "UTC: %s", asctime(ptm));
    fputs(szTimeStr, assertLogFP);

    fputs(szTemp, assertLogFP);

    fclose(assertLogFP);
  }

  // if the environment variable "NS_SILENT_ASSERTS" is set to a value like "1", "on", "true", "enable" or "yes"
  // the assert handler will never show a GUI that may block the application from continuing to run
  // this should be set on machines that run tests which should never get stuck but rather crash asap
  bool bSilentAsserts = false;

  if (nsEnvironmentVariableUtils::IsVariableSet("NS_SILENT_ASSERTS"))
  {
    bSilentAsserts = nsEnvironmentVariableUtils::GetValueInt("NS_SILENT_ASSERTS", bSilentAsserts ? 1 : 0) != 0;
  }

  if (bSilentAsserts)
    return true;

#if NS_ENABLED(NS_PLATFORM_WINDOWS)

    // make sure the cursor is definitely shown, since the user must be able to click buttons
#  if NS_ENABLED(NS_PLATFORM_WINDOWS_UWP)
    // Todo: Use modern Windows API to show cursor in current window.
    // http://stackoverflow.com/questions/37956628/change-mouse-pointer-in-uwp-app
#  else
  nsInt32 iHideCursor = 1;
  while (ShowCursor(true) < 0)
    ++iHideCursor;
#  endif

#  if NS_ENABLED(NS_COMPILE_FOR_DEBUG)

  nsInt32 iRes = _CrtDbgReport(_CRT_ASSERT, szSourceFile, uiLine, nullptr, "'%s'\nFunction: %s\nMessage: %s", szExpression, szFunction, szAssertMsg);

  // currently we will ALWAYS trigger the breakpoint / crash (except for when the user presses 'ignore')
  if (iRes == 0)
  {
    // when the user ignores the assert, restore the cursor show/hide state to the previous count
#    if NS_ENABLED(NS_PLATFORM_WINDOWS_UWP)
    // Todo: Use modern Windows API to restore cursor.
#    else
    for (nsInt32 i = 0; i < iHideCursor; ++i)
      ShowCursor(false);
#    endif

    return false;
  }

#  else


#    if NS_ENABLED(NS_PLATFORM_WINDOWS_DESKTOP)
  MessageBoxA(nullptr, szTemp, "Assertion", MB_ICONERROR);
#    endif

#  endif

#endif

  // always do a debug-break
  // in release-builds this will just crash the app
  return true;
}

static nsAssertHandler g_AssertHandler = &nsDefaultAssertHandler;

nsAssertHandler nsGetAssertHandler()
{
  return g_AssertHandler;
}

void nsSetAssertHandler(nsAssertHandler handler)
{
  g_AssertHandler = handler;
}

bool nsFailedCheck(const char* szSourceFile, nsUInt32 uiLine, const char* szFunction, const char* szExpression, const char* szMsg)
{
  // always do a debug-break if no assert handler is installed
  if (g_AssertHandler == nullptr)
    return true;

  return (*g_AssertHandler)(szSourceFile, uiLine, szFunction, szExpression, szMsg);
}

bool nsFailedCheck(const char* szSourceFile, nsUInt32 uiLine, const char* szFunction, const char* szExpression, const class nsFormatString& msg)
{
  nsStringBuilder tmp;
  return nsFailedCheck(szSourceFile, uiLine, szFunction, szExpression, msg.GetTextCStr(tmp));
}


NS_STATICLINK_FILE(Foundation, Foundation_Basics_Assert);
