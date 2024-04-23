/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Reflection/Implementation/DynamicRTTI.h>
#include <Foundation/Reflection/Implementation/RTTI.h>

bool nsReflectedClass::IsInstanceOf(const nsRTTI* pType) const
{
  return GetDynamicRTTI()->IsDerivedFrom(pType);
}


NS_STATICLINK_FILE(Foundation, Foundation_Reflection_Implementation_DynamicRTTI);
