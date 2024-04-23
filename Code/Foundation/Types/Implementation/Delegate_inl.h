/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */

#include <Foundation/Types/Implementation/DelegateHelper_inl.h>

template <typename Function>
NS_ALWAYS_INLINE nsDelegate<Function> nsMakeDelegate(Function* pFunction)
{
  return nsDelegate<Function>(pFunction);
}

template <typename Method, typename Class>
NS_ALWAYS_INLINE typename nsMakeDelegateHelper<Method>::DelegateType nsMakeDelegate(Method method, Class* pClass)
{
  return typename nsMakeDelegateHelper<Method>::DelegateType(method, pClass);
}
