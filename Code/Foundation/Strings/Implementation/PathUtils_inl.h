/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

NS_ALWAYS_INLINE bool nsPathUtils::IsPathSeparator(nsUInt32 c)
{
  return (c == '/' || c == '\\');
}
