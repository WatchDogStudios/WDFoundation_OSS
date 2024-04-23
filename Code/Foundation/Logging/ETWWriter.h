/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Logging/Log.h>

namespace nsLogWriter
{

  /// \brief A simple log writer that outputs all log messages to the ns ETW provider.
  class NS_FOUNDATION_DLL ETW
  {
  public:
    /// \brief Register this at nsLog to write all log messages to ETW.
    static void LogMessageHandler(const nsLoggingEventData& eventData);

    /// \brief Log Message to ETW.
    static void LogMessage(nsLogMsgType::Enum eventType, nsUInt8 uiIndentation, nsStringView sText);
  };
} // namespace nsLogWriter
