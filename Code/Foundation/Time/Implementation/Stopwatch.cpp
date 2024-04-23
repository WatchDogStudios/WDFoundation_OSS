/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Time/Stopwatch.h>

nsStopwatch::nsStopwatch()
{
  m_LastCheckpoint = nsTime::Now();

  StopAndReset();
  Resume();
}

void nsStopwatch::StopAndReset()
{
  m_TotalDuration = nsTime::MakeZero();
  m_bRunning = false;
}

void nsStopwatch::Resume()
{
  if (m_bRunning)
    return;

  m_bRunning = true;
  m_LastUpdate = nsTime::Now();
}

void nsStopwatch::Pause()
{
  if (!m_bRunning)
    return;

  m_bRunning = false;

  m_TotalDuration += nsTime::Now() - m_LastUpdate;
}

nsTime nsStopwatch::GetRunningTotal() const
{
  if (m_bRunning)
  {
    const nsTime tNow = nsTime::Now();

    m_TotalDuration += tNow - m_LastUpdate;
    m_LastUpdate = tNow;
  }

  return m_TotalDuration;
}

nsTime nsStopwatch::Checkpoint()
{
  const nsTime tNow = nsTime::Now();

  const nsTime tDiff = tNow - m_LastCheckpoint;
  m_LastCheckpoint = tNow;

  return tDiff;
}



NS_STATICLINK_FILE(Foundation, Foundation_Time_Implementation_Stopwatch);
