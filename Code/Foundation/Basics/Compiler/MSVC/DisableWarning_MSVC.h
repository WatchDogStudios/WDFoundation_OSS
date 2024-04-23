/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */

#ifdef NS_MSVC_WARNING_NUMBER

#  if NS_ENABLED(NS_COMPILER_MSVC)

#    pragma warning(push)
#    pragma warning(disable \
                    : NS_MSVC_WARNING_NUMBER)

#  endif

#  undef NS_MSVC_WARNING_NUMBER

#endif
