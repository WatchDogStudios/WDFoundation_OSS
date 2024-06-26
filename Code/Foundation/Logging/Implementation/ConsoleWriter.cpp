/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Logging/ConsoleWriter.h>
#include <Foundation/Time/Timestamp.h>

#if NS_ENABLED(NS_PLATFORM_ANDROID)
#  include <android/log.h>
#  define printf(...) __android_log_print(ANDROID_LOG_DEBUG, "nsEngine", __VA_ARGS__)
#endif

#if NS_ENABLED(NS_PLATFORM_WINDOWS)
#  include <Foundation/Basics/Platform/Win/IncludeWindows.h>

static void SetConsoleColor(WORD ui)
{
#  if NS_DISABLED(NS_PLATFORM_WINDOWS_UWP)
  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ui);
#  endif
}
#elif NS_ENABLED(NS_PLATFORM_OSX) || NS_ENABLED(NS_PLATFORM_LINUX) || NS_ENABLED(NS_PLATFORM_ANDROID)
static void SetConsoleColor(nsUInt8 ui) {}
#else
#  error "Unknown Platform."
static void SetConsoleColor(nsUInt8 ui) {}
#endif

nsLog::TimestampMode nsLogWriter::Console::s_TimestampMode = nsLog::TimestampMode::None;

void nsLogWriter::Console::LogMessageHandler(const nsLoggingEventData& eventData)
{
  nsStringBuilder sTimestamp;
  nsLog::GenerateFormattedTimestamp(s_TimestampMode, sTimestamp);

  static nsMutex WriterLock; // will only be created if this writer is used at all
  NS_LOCK(WriterLock);

  if (eventData.m_EventType == nsLogMsgType::BeginGroup)
    printf("\n");

  for (nsUInt32 i = 0; i < eventData.m_uiIndentation; ++i)
    printf(" ");

  nsStringBuilder sTemp1, sTemp2;

  switch (eventData.m_EventType)
  {
    case nsLogMsgType::Flush:
      fflush(stdout);
      break;

    case nsLogMsgType::BeginGroup:
      SetConsoleColor(0x02);
      printf("+++++ %s (%s) +++++\n", eventData.m_sText.GetData(sTemp1), eventData.m_sTag.GetData(sTemp2));
      break;

    case nsLogMsgType::EndGroup:
      SetConsoleColor(0x02);
#if NS_ENABLED(NS_COMPILE_FOR_DEVELOPMENT)
      printf("----- %s (%.6f sec)-----\n\n", eventData.m_sText.GetData(sTemp1), eventData.m_fSeconds);
#else
      printf("----- %s (%s)-----\n\n", eventData.m_sText.GetData(sTemp1), "timing info not available");
#endif
      break;

    case nsLogMsgType::ErrorMsg:
      SetConsoleColor(0x0C);
      printf("%sError: %s\n", sTimestamp.GetData(), eventData.m_sText.GetData(sTemp1));
      fflush(stdout);
      break;

    case nsLogMsgType::SeriousWarningMsg:
      SetConsoleColor(0x0C);
      printf("%sSeriously: %s\n", sTimestamp.GetData(), eventData.m_sText.GetData(sTemp1));
      break;

    case nsLogMsgType::WarningMsg:
      SetConsoleColor(0x0E);
      printf("%sWarning: %s\n", sTimestamp.GetData(), eventData.m_sText.GetData(sTemp1));
      break;

    case nsLogMsgType::SuccessMsg:
      SetConsoleColor(0x0A);
      printf("%s%s\n", sTimestamp.GetData(), eventData.m_sText.GetData(sTemp1));
      fflush(stdout);
      break;

    case nsLogMsgType::InfoMsg:
      SetConsoleColor(0x07);
      printf("%s%s\n", sTimestamp.GetData(), eventData.m_sText.GetData(sTemp1));
      break;

    case nsLogMsgType::DevMsg:
      SetConsoleColor(0x08);
      printf("%s%s\n", sTimestamp.GetData(), eventData.m_sText.GetData(sTemp1));
      break;

    case nsLogMsgType::DebugMsg:
      SetConsoleColor(0x09);
      printf("%s%s\n", sTimestamp.GetData(), eventData.m_sText.GetData(sTemp1));
      break;

    default:
      SetConsoleColor(0x0D);
      printf("%s%s\n", sTimestamp.GetData(), eventData.m_sText.GetData(sTemp1));

      nsLog::Warning("Unknown Message Type {0}", eventData.m_EventType);
      break;
  }

  SetConsoleColor(0x07);
}

void nsLogWriter::Console::SetTimestampMode(nsLog::TimestampMode mode)
{
  s_TimestampMode = mode;
}
#if NS_ENABLED(NS_PLATFORM_ANDROID)
#  undef printf
#endif


NS_STATICLINK_FILE(Foundation, Foundation_Logging_Implementation_ConsoleWriter);
