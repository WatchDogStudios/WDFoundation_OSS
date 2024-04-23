/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Basics.h>

#if NS_ENABLED(NS_PLATFORM_WINDOWS)

#  include <Foundation/FoundationInternal.h>
#  include <Foundation/Logging/Log.h>

NS_FOUNDATION_INTERNAL_HEADER

class nsETWProvider
{
public:
  nsETWProvider();
  ~nsETWProvider();

  void LogMessge(nsLogMsgType::Enum eventType, nsUInt8 uiIndentation, nsStringView sText);

  static nsETWProvider& GetInstance();
};
#endif
