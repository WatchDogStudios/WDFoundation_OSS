/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Logging/Log.h>

namespace nsLogWriter
{
  /// \brief A simple log writer that writes out log messages using printf.
  class NS_FOUNDATION_DLL Console
  {
  public:
    /// \brief Register this at nsLog to write all log messages to the console using printf.
    static void LogMessageHandler(const nsLoggingEventData& eventData);

    /// \brief Allows to indicate in what form timestamps should be added to log messages.
    static void SetTimestampMode(nsLog::TimestampMode mode);

  private:
    static nsLog::TimestampMode s_TimestampMode;
  };
} // namespace nsLogWriter
