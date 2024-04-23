/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */

///
/// Implements nsProcessGroup by using nsProcess
///

#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

#include <Foundation/System/ProcessGroup.h>

struct nsProcessGroupImpl
{
  NS_DECLARE_POD_TYPE();
};

nsProcessGroup::nsProcessGroup(nsStringView sGroupName)
{
}

nsProcessGroup::~nsProcessGroup()
{
  TerminateAll().IgnoreResult();
}

nsResult nsProcessGroup::Launch(const nsProcessOptions& opt)
{
  nsProcess& process = m_Processes.ExpandAndGetRef();
  return process.Launch(opt);
}

nsResult nsProcessGroup::WaitToFinish(nsTime timeout /*= nsTime::MakeZero()*/)
{
  for (auto& process : m_Processes)
  {
    if (process.GetState() != nsProcessState::Finished && process.WaitToFinish(timeout).Failed())
    {
      return NS_FAILURE;
    }
  }

  return NS_SUCCESS;
}

nsResult nsProcessGroup::TerminateAll(nsInt32 iForcedExitCode /*= -2*/)
{
  auto result = NS_SUCCESS;
  for (auto& process : m_Processes)
  {
    if (process.GetState() == nsProcessState::Running && process.Terminate().Failed())
    {
      result = NS_FAILURE;
    }
  }

  return result;
}
