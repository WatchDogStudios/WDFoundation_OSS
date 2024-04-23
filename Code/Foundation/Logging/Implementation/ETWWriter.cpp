/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Logging/ETWWriter.h>

#if NS_ENABLED(NS_PLATFORM_WINDOWS)
#  include <Foundation/Basics/Platform/Win/IncludeWindows.h>
#  include <Foundation/Logging/Implementation/Win/ETWProvider_win.h>

void nsLogWriter::ETW::LogMessageHandler(const nsLoggingEventData& eventData)
{
  if (eventData.m_EventType == nsLogMsgType::Flush)
    return;

  nsETWProvider::GetInstance().LogMessge(eventData.m_EventType, eventData.m_uiIndentation, eventData.m_sText);
}

void nsLogWriter::ETW::LogMessage(nsLogMsgType::Enum eventType, nsUInt8 uiIndentation, nsStringView sText)
{
  if (eventType == nsLogMsgType::Flush)
    return;

  nsETWProvider::GetInstance().LogMessge(eventType, uiIndentation, sText);
}

#else

void nsLogWriter::ETW::LogMessageHandler(const nsLoggingEventData& eventData) {}

void nsLogWriter::ETW::LogMessage(nsLogMsgType::Enum eventType, nsUInt8 uiIndentation, nsStringView sText) {}

#endif

NS_STATICLINK_FILE(Foundation, Foundation_Logging_Implementation_ETWWriter);
