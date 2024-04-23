/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

#include <Foundation/Logging/Log.h>
#include <Foundation/System/Process.h>
#include <Foundation/Threading/Thread.h>
#include <Foundation/Threading/ThreadUtils.h>

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/wait.h>
#include <unistd.h>

NS_DEFINE_AS_POD_TYPE(struct pollfd);

namespace
{
  nsResult AddFdFlags(int fd, int addFlags)
  {
    int flags = fcntl(fd, F_GETFD);
    flags |= addFlags;
    if (fcntl(fd, F_SETFD, flags) != 0)
    {
      nsLog::Error("Failed to set flags on {}: {}", fd, errno);
      return NS_FAILURE;
    }
    return NS_SUCCESS;
  }
} // namespace

struct nsProcessImpl
{
  ~nsProcessImpl()
  {
    StopStreamWatcher();
  }

  pid_t m_childPid = -1;
  bool m_exitCodeAvailable = false;
  bool m_processSuspended = false;

  struct StdStreamInfo
  {
    int fd;
    nsDelegate<void(nsStringView)> callback;
  };
  nsHybridArray<StdStreamInfo, 2> m_streams;
  nsDynamicArray<nsStringBuilder> m_overflowBuffers;
  nsUniquePtr<nsOSThread> m_streamWatcherThread;
  int m_wakeupPipeReadEnd = -1;
  int m_wakeupPipeWriteEnd = -1;

  static void* StreamWatcherThread(void* context)
  {
    nsProcessImpl* self = reinterpret_cast<nsProcessImpl*>(context);
    char buffer[4096];

    nsHybridArray<struct pollfd, 3> pollfds;

    pollfds.PushBack({self->m_wakeupPipeReadEnd, POLLIN, 0});
    for (StdStreamInfo& stream : self->m_streams)
    {
      pollfds.PushBack({stream.fd, POLLIN, 0});
    }

    bool run = true;
    while (run)
    {
      int result = poll(pollfds.GetData(), pollfds.GetCount(), -1);
      if (result > 0)
      {
        // Result at index 0 is special and means there was a WakeUp
        if (pollfds[0].revents != 0)
        {
          run = false;
        }

        for (nsUInt32 i = 1; i < pollfds.GetCount(); ++i)
        {
          if (pollfds[i].revents != 0)
          {
            nsStringBuilder& overflowBuffer = self->m_overflowBuffers[i - 1];
            StdStreamInfo& stream = self->m_streams[i - 1];
            pollfds[i].revents = 0;
            while (true)
            {
              ssize_t numBytes = read(stream.fd, buffer, NS_ARRAY_SIZE(buffer));
              if (numBytes < 0)
              {
                if (errno == EWOULDBLOCK)
                {
                  break;
                }
                nsLog::Error("Process Posix read error on {}: {}", stream.fd, errno);
                return nullptr;
              }
              if (numBytes == 0)
              {
                break;
              }

              const char* szCurrentPos = buffer;
              const char* szEndPos = buffer + numBytes;
              while (szCurrentPos < szEndPos)
              {
                const char* szFound = nsStringUtils::FindSubString(szCurrentPos, "\n", szEndPos);
                if (szFound)
                {
                  if (overflowBuffer.IsEmpty())
                  {
                    // If there is nothing in the overflow buffer this is a complete line and can be fired as is.
                    stream.callback(nsStringView(szCurrentPos, szFound + 1));
                  }
                  else
                  {
                    // We have data in the overflow buffer so this is the final part of a partial line so we need to complete and fire the overflow buffer.
                    overflowBuffer.Append(nsStringView(szCurrentPos, szFound + 1));
                    stream.callback(overflowBuffer);
                    overflowBuffer.Clear();
                  }
                  szCurrentPos = szFound + 1;
                }
                else
                {
                  // This is either the start or a middle segment of a line, append to overflow buffer.
                  overflowBuffer.Append(nsStringView(szCurrentPos, szEndPos));
                  szCurrentPos = szEndPos;
                }
              }
            }
          }
        }
      }
      else if (result < 0)
      {
        nsLog::Error("poll error {}", errno);
        break;
      }
    }

    for (nsUInt32 i = 0; i < self->m_streams.GetCount(); ++i)
    {
      nsStringBuilder& overflowBuffer = self->m_overflowBuffers[i];
      if (!overflowBuffer.IsEmpty())
      {
        self->m_streams[i].callback(overflowBuffer);
        overflowBuffer.Clear();
      }
    }

    return nullptr;
  }

  nsResult StartStreamWatcher()
  {
    int wakeupPipe[2] = {-1, -1};
    if (pipe(wakeupPipe) < 0)
    {
      nsLog::Error("Failed to setup wakeup pipe {}", errno);
      return NS_FAILURE;
    }
    else
    {
      m_wakeupPipeReadEnd = wakeupPipe[0];
      m_wakeupPipeWriteEnd = wakeupPipe[1];
      if (AddFdFlags(m_wakeupPipeReadEnd, O_NONBLOCK | O_CLOEXEC).Failed() ||
          AddFdFlags(m_wakeupPipeWriteEnd, O_NONBLOCK | O_CLOEXEC).Failed())
      {
        close(m_wakeupPipeReadEnd);
        m_wakeupPipeReadEnd = -1;
        close(m_wakeupPipeWriteEnd);
        m_wakeupPipeWriteEnd = -1;
        return NS_FAILURE;
      }
    }

    m_streamWatcherThread = NS_DEFAULT_NEW(nsOSThread, &StreamWatcherThread, this, "StdStrmWtch");
    m_streamWatcherThread->Start();

    return NS_SUCCESS;
  }

  void StopStreamWatcher()
  {
    if (m_streamWatcherThread)
    {
      char c = 0;
      NS_IGNORE_UNUSED(write(m_wakeupPipeWriteEnd, &c, 1));
      m_streamWatcherThread->Join();
      m_streamWatcherThread = nullptr;
    }
    close(m_wakeupPipeReadEnd);
    close(m_wakeupPipeWriteEnd);
    m_wakeupPipeReadEnd = -1;
    m_wakeupPipeWriteEnd = -1;
  }

  void AddStream(int fd, const nsDelegate<void(nsStringView)>& callback)
  {
    m_streams.PushBack({fd, callback});
    m_overflowBuffers.SetCount(m_streams.GetCount());
  }

  static nsResult StartChildProcess(const nsProcessOptions& opt, pid_t& outPid, bool suspended, int& outStdOutFd, int& outStdErrFd)
  {
    int stdoutPipe[2] = {-1, -1};
    int stderrPipe[2] = {-1, -1};

    if (opt.m_onStdOut.IsValid())
    {
      if (pipe(stdoutPipe) < 0)
      {
        return NS_FAILURE;
      }
    }

    if (opt.m_onStdError.IsValid())
    {
      if (pipe(stderrPipe) < 0)
      {
        return NS_FAILURE;
      }
    }

    pid_t childPid = fork();
    if (childPid < 0)
    {
      return NS_FAILURE;
    }

    if (childPid == 0) // We are the child
    {
      if (suspended)
      {
        if (raise(SIGSTOP) < 0)
        {
          _exit(-1);
        }
      }

      if (opt.m_bHideConsoleWindow == true)
      {
        // Redirect STDIN to /dev/null
        int stdinReplace = open("/dev/null", O_RDONLY);
        dup2(stdinReplace, STDIN_FILENO);
        close(stdinReplace);

        if (!opt.m_onStdOut.IsValid())
        {
          int stdoutReplace = open("/dev/null", O_WRONLY);
          dup2(stdoutReplace, STDOUT_FILENO);
          close(stdoutReplace);
        }

        if (!opt.m_onStdError.IsValid())
        {
          int stderrReplace = open("/dev/null", O_WRONLY);
          dup2(stderrReplace, STDERR_FILENO);
          close(stderrReplace);
        }
      }
      else
      {
        // TODO: Launch a x-terminal-emulator with the command and somehow redirect STDOUT, etc?
        NS_ASSERT_NOT_IMPLEMENTED;
      }

      if (opt.m_onStdOut.IsValid())
      {
        close(stdoutPipe[0]);               // We don't need the read end of the pipe in the child process
        dup2(stdoutPipe[1], STDOUT_FILENO); // redirect the write end to STDOUT
        close(stdoutPipe[1]);
      }

      if (opt.m_onStdError.IsValid())
      {
        close(stderrPipe[0]);               // We don't need the read end of the pipe in the child process
        dup2(stderrPipe[1], STDERR_FILENO); // redirect the write end to STDERR
        close(stderrPipe[1]);
      }

      nsHybridArray<char*, 9> args;

      for (const nsString& arg : opt.m_Arguments)
      {
        args.PushBack(const_cast<char*>(arg.GetData()));
      }
      args.PushBack(nullptr);

      if (!opt.m_sWorkingDirectory.IsEmpty())
      {
        if (chdir(opt.m_sWorkingDirectory.GetData()) < 0)
        {
          _exit(-1); // Failed to change working directory
        }
      }

      if (execv(opt.m_sProcess.GetData(), args.GetData()) < 0)
      {
        _exit(-1);
      }
    }
    else
    {
      outPid = childPid;

      if (opt.m_onStdOut.IsValid())
      {
        close(stdoutPipe[1]); // Don't need the write end in the parent process
        outStdOutFd = stdoutPipe[0];
      }

      if (opt.m_onStdError.IsValid())
      {
        close(stderrPipe[1]); // Don't need the write end in the parent process
        outStdErrFd = stderrPipe[0];
      }
    }

    return NS_SUCCESS;
  }
};

nsProcess::nsProcess()
{
  m_pImpl = NS_DEFAULT_NEW(nsProcessImpl);
}

nsProcess::~nsProcess()
{
  if (GetState() == nsProcessState::Running)
  {
    nsLog::Dev("Process still running - terminating '{}'", m_sProcess);

    Terminate().IgnoreResult();
  }

  // Explicitly clear the implementation here so that member
  // state (e.g. delegates) used by the impl survives the implementation.
  m_pImpl.Clear();
}

nsResult nsProcess::Execute(const nsProcessOptions& opt, nsInt32* out_iExitCode /*= nullptr*/)
{
  pid_t childPid = 0;
  int stdoutFd = -1;
  int stderrFd = -1;
  if (nsProcessImpl::StartChildProcess(opt, childPid, false, stdoutFd, stderrFd).Failed())
  {
    return NS_FAILURE;
  }

  nsProcessImpl impl;
  if (stdoutFd >= 0)
  {
    impl.AddStream(stdoutFd, opt.m_onStdOut);
  }

  if (stderrFd >= 0)
  {
    impl.AddStream(stderrFd, opt.m_onStdError);
  }

  if (stdoutFd >= 0 || stderrFd >= 0)
  {
    if (impl.StartStreamWatcher().Failed())
    {
      return NS_FAILURE;
    }
  }

  int childStatus = -1;
  pid_t waitedPid = waitpid(childPid, &childStatus, 0);
  if (waitedPid < 0)
  {
    return NS_FAILURE;
  }
  if (out_iExitCode != nullptr)
  {
    if (WIFEXITED(childStatus))
    {
      *out_iExitCode = WEXITSTATUS(childStatus);
    }
    else
    {
      *out_iExitCode = -1;
    }
  }
  return NS_SUCCESS;
}

nsResult nsProcess::Launch(const nsProcessOptions& opt, nsBitflags<nsProcessLaunchFlags> launchFlags /*= nsProcessLaunchFlags::None*/)
{
  NS_ASSERT_DEV(m_pImpl->m_childPid == -1, "Can not reuse an instance of nsProcess");

  int stdoutFd = -1;
  int stderrFd = -1;

  if (nsProcessImpl::StartChildProcess(opt, m_pImpl->m_childPid, launchFlags.IsSet(nsProcessLaunchFlags::Suspended), stdoutFd, stderrFd).Failed())
  {
    return NS_FAILURE;
  }

  m_pImpl->m_exitCodeAvailable = false;
  m_pImpl->m_processSuspended = launchFlags.IsSet(nsProcessLaunchFlags::Suspended);

  if (stdoutFd >= 0)
  {
    m_pImpl->AddStream(stdoutFd, opt.m_onStdOut);
  }

  if (stderrFd >= 0)
  {
    m_pImpl->AddStream(stderrFd, opt.m_onStdError);
  }

  if (stdoutFd >= 0 || stderrFd >= 0)
  {
    if (m_pImpl->StartStreamWatcher().Failed())
    {
      return NS_FAILURE;
    }
  }

  if (launchFlags.IsSet(nsProcessLaunchFlags::Detached))
  {
    Detach();
  }

  return NS_SUCCESS;
}

nsResult nsProcess::ResumeSuspended()
{
  if (m_pImpl->m_childPid < 0 || !m_pImpl->m_processSuspended)
  {
    return NS_FAILURE;
  }

  if (kill(m_pImpl->m_childPid, SIGCONT) < 0)
  {
    return NS_FAILURE;
  }
  m_pImpl->m_processSuspended = false;
  return NS_SUCCESS;
}

nsResult nsProcess::WaitToFinish(nsTime timeout /*= nsTime::MakeZero()*/)
{
  int childStatus = 0;
  NS_SCOPE_EXIT(m_pImpl->StopStreamWatcher());

  if (timeout.IsZero())
  {
    if (waitpid(m_pImpl->m_childPid, &childStatus, 0) < 0)
    {
      return NS_FAILURE;
    }
  }
  else
  {
    int waitResult = 0;
    nsTime startWait = nsTime::Now();
    while (true)
    {
      waitResult = waitpid(m_pImpl->m_childPid, &childStatus, WNOHANG);
      if (waitResult < 0)
      {
        return NS_FAILURE;
      }
      if (waitResult > 0)
      {
        break;
      }
      nsTime timeSpent = nsTime::Now() - startWait;
      if (timeSpent > timeout)
      {
        return NS_FAILURE;
      }
      nsThreadUtils::Sleep(nsMath::Min(nsTime::MakeFromMilliseconds(100.0), timeout - timeSpent));
    }
  }

  if (WIFEXITED(childStatus))
  {
    m_iExitCode = WEXITSTATUS(childStatus);
  }
  else
  {
    m_iExitCode = -1;
  }
  m_pImpl->m_exitCodeAvailable = true;

  return NS_SUCCESS;
}

nsResult nsProcess::Terminate()
{
  if (m_pImpl->m_childPid == -1)
  {
    return NS_FAILURE;
  }

  NS_SCOPE_EXIT(m_pImpl->StopStreamWatcher());

  if (kill(m_pImpl->m_childPid, SIGKILL) < 0)
  {
    if (errno != ESRCH) // ESRCH = Process does not exist
    {
      return NS_FAILURE;
    }
  }
  m_pImpl->m_exitCodeAvailable = true;
  m_iExitCode = -1;

  return NS_SUCCESS;
}

nsProcessState nsProcess::GetState() const
{
  if (m_pImpl->m_childPid == -1)
  {
    return nsProcessState::NotStarted;
  }

  if (m_pImpl->m_exitCodeAvailable)
  {
    return nsProcessState::Finished;
  }

  int childStatus = -1;
  int waitResult = waitpid(m_pImpl->m_childPid, &childStatus, WNOHANG);
  if (waitResult > 0)
  {
    m_iExitCode = WEXITSTATUS(childStatus);
    m_pImpl->m_exitCodeAvailable = true;

    m_pImpl->StopStreamWatcher();

    return nsProcessState::Finished;
  }

  return nsProcessState::Running;
}

void nsProcess::Detach()
{
  m_pImpl->m_childPid = -1;
}

nsOsProcessHandle nsProcess::GetProcessHandle() const
{
  NS_ASSERT_DEV(false, "There is no process handle on posix");
  return nullptr;
}

nsOsProcessID nsProcess::GetProcessID() const
{
  NS_ASSERT_DEV(m_pImpl->m_childPid != -1, "No ProcessID available");
  return m_pImpl->m_childPid;
}

nsOsProcessID nsProcess::GetCurrentProcessID()
{
  return getpid();
}
