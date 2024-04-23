/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */

#ifdef NS_GCC_WARNING_NAME

#  if NS_ENABLED(NS_COMPILER_GCC)

#    pragma GCC diagnostic push
_Pragma(NS_STRINGIZE(GCC diagnostic ignored NS_GCC_WARNING_NAME))

#  endif

#  undef NS_GCC_WARNING_NAME

#endif
