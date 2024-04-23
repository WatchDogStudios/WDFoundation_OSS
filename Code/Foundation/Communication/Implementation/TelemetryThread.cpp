/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Communication/Telemetry.h>
#include <Foundation/Threading/Thread.h>

class nsTelemetryThread : public nsThread
{
public:
  nsTelemetryThread()
    : nsThread("nsTelemetryThread")
  {
    m_bKeepRunning = true;
  }

  volatile bool m_bKeepRunning;

private:
  virtual nsUInt32 Run()
  {
    nsTime LastPing;

    while (m_bKeepRunning)
    {
      nsTelemetry::UpdateNetwork();

      // Send a Ping every once in a while
      if (nsTelemetry::s_ConnectionMode == nsTelemetry::Client)
      {
        nsTime tNow = nsTime::Now();

        if (tNow - LastPing > nsTime::MakeFromMilliseconds(500))
        {
          LastPing = tNow;

          nsTelemetry::UpdateServerPing();
        }
      }

      nsThreadUtils::Sleep(nsTime::MakeFromMilliseconds(10));
    }

    return 0;
  }
};

static nsTelemetryThread* g_pBroadcastThread = nullptr;
nsMutex nsTelemetry::s_TelemetryMutex;


nsMutex& nsTelemetry::GetTelemetryMutex()
{
  return s_TelemetryMutex;
}

void nsTelemetry::StartTelemetryThread()
{
  if (!g_pBroadcastThread)
  {
    g_pBroadcastThread = NS_DEFAULT_NEW(nsTelemetryThread);
    g_pBroadcastThread->Start();
  }
}

void nsTelemetry::StopTelemetryThread()
{
  if (g_pBroadcastThread)
  {
    g_pBroadcastThread->m_bKeepRunning = false;
    g_pBroadcastThread->Join();

    NS_DEFAULT_DELETE(g_pBroadcastThread);
  }
}



NS_STATICLINK_FILE(Foundation, Foundation_Communication_Implementation_TelemetryThread);
