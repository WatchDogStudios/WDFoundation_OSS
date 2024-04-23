/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */

#pragma once

#include <Foundation/Basics.h>

/// \file

/// \brief Macro to execute a piece of code when the current scope closes.
#define NS_SCOPE_EXIT(code) auto NS_CONCAT(scopeExit_, NS_SOURCE_LINE) = nsMakeScopeExit([&]() { code; })

/// \internal Helper class to implement NS_SCOPE_EXIT
template <typename T>
struct nsScopeExit
{
  NS_ALWAYS_INLINE nsScopeExit(T&& func)
    : m_func(std::forward<T>(func))
  {
  }

  NS_ALWAYS_INLINE ~nsScopeExit() { m_func(); }

  T m_func;
};

/// \internal Helper function to implement NS_SCOPE_EXIT
template <typename T>
NS_ALWAYS_INLINE nsScopeExit<T> nsMakeScopeExit(T&& func)
{
  return nsScopeExit<T>(std::forward<T>(func));
}
