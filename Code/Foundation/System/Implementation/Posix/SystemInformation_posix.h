/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

#include <Foundation/IO/OSFile.h>
#include <unistd.h>

bool nsSystemInformation::IsDebuggerAttached()
{
  nsOSFile status;
  if (status.Open("/proc/self/status", nsFileOpenMode::Read).Failed())
  {
    return false;
  }


  char buffer[2048];
  nsUInt64 numBytesRead = status.Read(buffer, NS_ARRAY_SIZE(buffer));
  status.Close();


  nsStringView contents(buffer, numBytesRead);
  const char* tracerPid = contents.FindSubString("TracerPid:");
  if (tracerPid == nullptr)
  {
    return false;
  }

  tracerPid += 10; // Skip TracerPid:

  while (*tracerPid == ' ' || *tracerPid == '\t')
  {
    tracerPid++;
  }

  return *tracerPid == '0' ? false : true;
}

void nsSystemInformation::Initialize()
{
  if (s_SystemInformation.m_bIsInitialized)
    return;

  s_SystemInformation.m_CpuFeatures.Detect();

  // Get system information via various APIs
  s_SystemInformation.m_uiCPUCoreCount = sysconf(_SC_NPROCESSORS_ONLN);

  nsUInt64 uiPageCount = sysconf(_SC_PHYS_PAGES);
  nsUInt64 uiPageSize = sysconf(_SC_PAGE_SIZE);

  s_SystemInformation.m_uiInstalledMainMemory = uiPageCount * uiPageSize;
  s_SystemInformation.m_uiMemoryPageSize = uiPageSize;

  // Not correct for 32 bit process on 64 bit system
#if NS_ENABLED(NS_PLATFORM_64BIT)
  s_SystemInformation.m_bB64BitOS = true;
#else
  s_SystemInformation.m_bB64BitOS = false;
#  if NS_ENABLED(NS_PLATFORM_OSX)
#    error "32 Bit builds are not supported on OSX"
#  endif
#endif

#if defined BUILDSYSTEM_BUILDTYPE
  s_SystemInformation.m_szBuildConfiguration = BUILDSYSTEM_BUILDTYPE;
#else
  s_SystemInformation.m_szBuildConfiguration = "undefined";
#endif

  // Each posix system should have its correct name so they can be distinguished.
#if NS_ENABLED(NS_PLATFORM_LINUX)
  s_SystemInformation.m_szPlatformName = "Linux";
#elif NS_ENABLED(NS_PLATFORM_ANDROID)
  s_SystemInformation.m_szPlatformName = "Android";
#else
#  error "Platform name not defined on current posix platform"
#endif

  //  Get host name
  if (gethostname(s_SystemInformation.m_sHostName, sizeof(s_SystemInformation.m_sHostName)) == -1)
  {
    strcpy(s_SystemInformation.m_sHostName, "");
  }

  s_SystemInformation.m_bIsInitialized = true;
}

nsUInt64 nsSystemInformation::GetAvailableMainMemory() const
{
  return static_cast<nsUInt64>(sysconf(_SC_AVPHYS_PAGES)) * static_cast<nsUInt64>(sysconf(_SC_PAGESIZE));
}

float nsSystemInformation::GetCPUUtilization() const
{
  NS_ASSERT_NOT_IMPLEMENTED;
  return 0.0f;
}
