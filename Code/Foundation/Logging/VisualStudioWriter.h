/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Logging/Log.h>

namespace nsLogWriter
{

  /// \brief A simple log writer that outputs all log messages to visual studios output window
  class NS_FOUNDATION_DLL VisualStudio
  {
  public:
    /// \brief Register this at nsLog to write all log messages to visual studios output window.
    static void LogMessageHandler(const nsLoggingEventData& eventData);
  };
} // namespace nsLogWriter
