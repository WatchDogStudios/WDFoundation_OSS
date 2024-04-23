/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <FoundationTest/FoundationTestPCH.h>

#define INT_DECLARE(name, n) int name = n;

namespace
{
  NS_EXPAND_ARGS_WITH_INDEX(INT_DECLARE, heinz, klaus);
}

NS_CREATE_SIMPLE_TEST(Basics, BlackMagic)
{
  NS_TEST_INT(heinz, 0);
  NS_TEST_INT(klaus, 1);
}
