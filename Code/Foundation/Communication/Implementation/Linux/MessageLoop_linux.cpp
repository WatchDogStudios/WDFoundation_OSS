/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#if NS_ENABLED(NS_PLATFORM_LINUX)

#  include <Foundation/Communication/Implementation/Linux/MessageLoop_linux.h>

#  include <Foundation/Communication/Implementation/Linux/PipeChannel_linux.h>
#  include <Foundation/Logging/Log.h>

#  include <fcntl.h>
#  include <poll.h>
#  include <unistd.h>

nsMessageLoop_linux::nsMessageLoop_linux()
{
  int fds[2];
  if (pipe2(fds, O_NONBLOCK | O_CLOEXEC) < 0)
  {
    nsLog::Error("[IPC]Failed to create wakeup pipe for nsMessageLoop_linux");
  }
  else
  {
    m_wakeupPipeReadEndFd = fds[0];
    m_wakeupPipeWriteEndFd = fds[1];

    // Setup always present wait info for magic index 0 = wake up
    m_pollInfos.PushBack({m_wakeupPipeReadEndFd, POLLIN, 0});
    m_waitInfos.PushBack({nullptr, WaitType::Accept});
  }
}

nsMessageLoop_linux::~nsMessageLoop_linux()
{
  StopUpdateThread();
  if (m_wakeupPipeReadEndFd >= 0)
  {
    close(m_wakeupPipeReadEndFd);
    close(m_wakeupPipeWriteEndFd);
  }
}

bool nsMessageLoop_linux::WaitForMessages(nsInt32 iTimeout, nsIpcChannel* pFilter)
{
  NS_ASSERT_ALWAYS(pFilter == nullptr, "Not implemented");

  while (m_numPendingPollModifications > 0)
  {
    nsThreadUtils::YieldTimeSlice();
  }

  nsLock lock{m_pollMutex};
  int result = poll(m_pollInfos.GetData(), m_pollInfos.GetCount(), iTimeout);
  if (result > 0)
  {
    // Result at index 0 is special and means there was a WakeUp
    if (m_pollInfos[0].revents != 0)
    {
      nsUInt8 wakeupByte;
      auto readResult = read(m_wakeupPipeReadEndFd, &wakeupByte, sizeof(wakeupByte));
      m_pollInfos[0].revents = 0;
      return true;
    }

    nsUInt32 numEvents = m_pollInfos.GetCount();
    for (nsUInt32 i = 1; i < numEvents;)
    {
      WaitInfo& waitInfo = m_waitInfos[i];
      struct pollfd& pollInfo = m_pollInfos[i];
      if (pollInfo.revents != 0)
      {
        switch (waitInfo.m_type)
        {
          case WaitType::Accept:
            waitInfo.m_pChannel->AcceptIncomingConnection();
            m_pollInfos.RemoveAtAndSwap(i);
            m_waitInfos.RemoveAtAndSwap(i);
            numEvents--;
            continue;
          case WaitType::Connect:
            waitInfo.m_pChannel->ProcessConnectSuccessfull();
            m_pollInfos.RemoveAtAndSwap(i);
            m_waitInfos.RemoveAtAndSwap(i);
            numEvents--;
            continue;
          case WaitType::IncomingMessage:
            waitInfo.m_pChannel->ProcessIncomingPackages();
            break;
          case WaitType::Send:
            waitInfo.m_pChannel->InternalSend();
            m_pollInfos.RemoveAtAndSwap(i);
            m_waitInfos.RemoveAtAndSwap(i);
            numEvents--;
            continue;
        }
        pollInfo.revents = 0;
      }
      ++i;
    }
    return true;
  }
  if (result < 0)
  {
    nsLog::Error("[IPC]nsMessageLoop_linux failed to poll events. Error {}", errno);
  }
  return false;
}

void nsMessageLoop_linux::RegisterWait(nsPipeChannel_linux* pChannel, WaitType type, int fd)
{
  nsLog::Debug("[IPC]nsMessageLoop_linux::RegisterWait({}}", (int)type);
  short int waitFlags = 0;
  switch (type)
  {
    case WaitType::Accept:
      waitFlags = POLLIN;
      break;
    case WaitType::Connect:
      waitFlags = POLLOUT;
      break;
    case WaitType::IncomingMessage:
      waitFlags = POLLIN;
      break;
    case WaitType::Send:
      waitFlags = POLLOUT;
      break;
  }

  m_numPendingPollModifications.Increment();
  NS_SCOPE_EXIT(m_numPendingPollModifications.Decrement());
  WakeUp();
  {
    nsLock lock{m_pollMutex};
    m_waitInfos.PushBack({pChannel, type});
    m_pollInfos.PushBack({fd, waitFlags, 0});
  }
}

void nsMessageLoop_linux::RemovePendingWaits(nsPipeChannel_linux* pChannel)
{
  m_numPendingPollModifications.Increment();
  NS_SCOPE_EXIT(m_numPendingPollModifications.Decrement());
  WakeUp();
  {
    nsLock lock{m_pollMutex};
    nsUInt32 waitCount = m_pollInfos.GetCount();
    for (nsUInt32 i = 0; i < waitCount;)
    {
      if (m_waitInfos[i].m_pChannel == pChannel)
      {
        m_waitInfos.RemoveAtAndSwap(i);
        m_pollInfos.RemoveAtAndSwap(i);
        waitCount--;
      }
      else
      {
        i++;
      }
    }
  }
}

void nsMessageLoop_linux::WakeUp()
{
  nsUInt8 wakeupByte = 0;
  int writeResult = write(m_wakeupPipeWriteEndFd, &wakeupByte, sizeof(wakeupByte));
}

#endif


NS_STATICLINK_FILE(Foundation, Foundation_Communication_Implementation_Linux_MessageLoop_linux);
