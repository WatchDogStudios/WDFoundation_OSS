/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

nsString nsEnvironmentVariableUtils::GetValueStringImpl(nsStringView sName, nsStringView sDefault)
{
  NS_ASSERT_NOT_IMPLEMENTED
  return "";
}

nsResult nsEnvironmentVariableUtils::SetValueStringImpl(nsStringView sName, nsStringView szValue)
{
  NS_ASSERT_NOT_IMPLEMENTED
  return NS_FAILURE;
}

bool nsEnvironmentVariableUtils::IsVariableSetImpl(nsStringView sName)
{
  return false;
}

nsResult nsEnvironmentVariableUtils::UnsetVariableImpl(nsStringView sName)
{
  return NS_FAILURE;
}
