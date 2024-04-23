/*
 *   Copyright (c) 2023 WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#ifndef NS_CONCAT

/// \brief Concatenates two strings, even when the strings are macros themselves
#define NS_CONCAT(x,y) NS_CONCAT_HELPER(x,y)
#define NS_CONCAT_HELPER(x,y) NS_CONCAT_HELPER2(x,y)
#define NS_CONCAT_HELPER2(x,y) x##y

#endif

#ifndef NS_STRINGIZE

/// \brief Turns some piece of code (usually some identifier name) into a string. Even works on macros.
#define NS_STRINGIZE(str) NS_STRINGIZE_HELPER(str)
#define NS_STRINGIZE_HELPER(x) #x

#endif

#ifndef NS_ON

/// \brief Used in conjunction with NS_ENABLED and NS_DISABLED for safe checks. Define something to NS_ON or NS_OFF to work with those macros.
#define NS_ON =

/// \brief Used in conjunction with NS_ENABLED and NS_DISABLED for safe checks. Define something to NS_ON or NS_OFF to work with those macros.
#define NS_OFF !

/// \brief Used in conjunction with NS_ON and NS_OFF for safe checks. Use #if NS_ENABLED(x) or #if NS_DISABLED(x) in conditional compilation.
#define NS_ENABLED(x) (1 NS_CONCAT(x,=) 1)

/// \brief Used in conjunction with NS_ON and NS_OFF for safe checks. Use #if NS_ENABLED(x) or #if NS_DISABLED(x) in conditional compilation.
#define NS_DISABLED(x) (1 NS_CONCAT(x,=) 2)

/// \brief Checks whether x AND y are both defined as NS_ON or NS_OFF. Usually used to check whether configurations overlap, to issue an error.
#define NS_IS_NOT_EXCLUSIVE(x, y) ((1 NS_CONCAT(x,=) 1) == (1 NS_CONCAT(y,=) 1))

#endif
