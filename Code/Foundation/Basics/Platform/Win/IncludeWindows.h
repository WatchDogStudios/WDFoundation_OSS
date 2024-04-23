/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Basics.h>

#define NS_INCLUDED_WINDOWS_H 1

#if NS_ENABLED(NS_PLATFORM_WINDOWS)
// this is important for code that wants to include winsock2.h later on
#  define _WINSOCKAPI_ /* Prevent inclusion of winsock.h in windows.h */

// already includes Windows.h, but defines important other things first
//#include <winsock2.h>

#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <Windows.h>

// unset windows macros
#  undef min
#  undef max
#  undef GetObject
#  undef ERROR
#  undef DeleteFile
#  undef CopyFile
#  undef DispatchMessage
#  undef PostMessage
#  undef SendMessage
#  undef OPAQUE
#  undef SetPort

#  include <Foundation/Basics/Platform/Win/MinWindows.h>

namespace nsMinWindows
{
  template <>
  struct ToNativeImpl<HINSTANCE>
  {
    using type = ::HINSTANCE;
    static NS_ALWAYS_INLINE ::HINSTANCE ToNative(HINSTANCE hInstance) { return reinterpret_cast<::HINSTANCE>(hInstance); }
  };

  template <>
  struct ToNativeImpl<HWND>
  {
    using type = ::HWND;
    static NS_ALWAYS_INLINE ::HWND ToNative(HWND hWnd) { return reinterpret_cast<::HWND>(hWnd); }
  };

  template <>
  struct FromNativeImpl<::HWND>
  {
    using type = HWND;
    static NS_ALWAYS_INLINE HWND FromNative(::HWND hWnd) { return reinterpret_cast<HWND>(hWnd); }
  };

  template <>
  struct FromNativeImpl<::HINSTANCE>
  {
    using type = HINSTANCE;
    static NS_ALWAYS_INLINE HINSTANCE FromNative(::HINSTANCE hInstance) { return reinterpret_cast<HINSTANCE>(hInstance); }
  };
} // namespace nsMinWindows
#endif
