/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#if NS_ENABLED(NS_SUPPORTS_PROCESSES)
// Include inline file
#  if NS_ENABLED(NS_PLATFORM_WINDOWS)
#    include <Foundation/System/Implementation/Win/ProcessGroup_win.h>
#  else
#    include <Foundation/System/Implementation/other/ProcessGroup_other.h>
#  endif

const nsHybridArray<nsProcess, 8>& nsProcessGroup::GetProcesses() const
{
  return m_Processes;
}

#endif

NS_STATICLINK_FILE(Foundation, Foundation_System_Implementation_ProcessGroup);
