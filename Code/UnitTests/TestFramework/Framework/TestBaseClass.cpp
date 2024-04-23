/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <TestFramework/TestFrameworkPCH.h>

#include <TestFramework/Framework/TestFramework.h>

NS_ENUMERABLE_CLASS_IMPLEMENTATION(nsTestBaseClass);

const char* nsTestBaseClass::GetSubTestName(nsInt32 iIdentifier) const
{
  if (iIdentifier < 0 || static_cast<std::size_t>(iIdentifier) > m_Entries.size())
  {
    nsLog::Error("Tried to access retrieve sub-test name using invalid identifier.");
    return "";
  }

  return m_Entries[iIdentifier].m_szName;
}

void nsTestBaseClass::UpdateConfiguration(nsTestConfiguration& ref_config) const
{
  // If the configuration hasn't been set yet this is the first instance of nsTestBaseClass being called
  // to fill in the configuration and we thus have to do so.
  // Derived classes can have more information (e.g.GPU info) and there is no way to know which instance
  // of nsTestBaseClass may have additional information so we ask all of them and each one early outs
  // if the information it knows about is already present.
  if (ref_config.m_uiInstalledMainMemory == 0)
  {
    const nsSystemInformation& pSysInfo = nsSystemInformation::Get();
    ref_config.m_uiInstalledMainMemory = pSysInfo.GetInstalledMainMemory();
    ref_config.m_uiMemoryPageSize = pSysInfo.GetMemoryPageSize();
    ref_config.m_uiCPUCoreCount = pSysInfo.GetCPUCoreCount();
    ref_config.m_sPlatformName = pSysInfo.GetPlatformName();
    ref_config.m_b64BitOS = pSysInfo.Is64BitOS();
    ref_config.m_b64BitApplication = NS_ENABLED(NS_PLATFORM_64BIT);
    ref_config.m_sBuildConfiguration = pSysInfo.GetBuildConfiguration();
    ref_config.m_iDateTime = nsTimestamp::CurrentTimestamp().GetInt64(nsSIUnitOfTime::Second);
    ref_config.m_iRCSRevision = nsTestFramework::GetInstance()->GetSettings().m_iRevision;
    ref_config.m_sHostName = pSysInfo.GetHostName();
  }
}

void nsTestBaseClass::MapImageNumberToString(const char* szTestName, const nsSubTestEntry& subTest, nsUInt32 uiImageNumber, nsStringBuilder& out_sString) const
{
  out_sString.Format("{0}_{1}_{2}", szTestName, subTest.m_szSubTestName, nsArgI(uiImageNumber, 3, true));
  out_sString.ReplaceAll(" ", "_");
}

void nsTestBaseClass::ClearSubTests()
{
  m_Entries.clear();
}

void nsTestBaseClass::AddSubTest(const char* szName, nsInt32 iIdentifier)
{
  NS_ASSERT_DEV(szName != nullptr, "Sub test name must not be nullptr");

  TestEntry e;
  e.m_szName = szName;
  e.m_iIdentifier = iIdentifier;

  m_Entries.push_back(e);
}

nsResult nsTestBaseClass::DoTestInitialization()
{
  try
  {
    if (InitializeTest() == NS_FAILURE)
    {
      nsTestFramework::Output(nsTestOutput::Error, "Test Initialization failed.");
      return NS_FAILURE;
    }
  }
  catch (...)
  {
    nsTestFramework::Output(nsTestOutput::Error, "Exception during test initialization.");
    return NS_FAILURE;
  }

  return NS_SUCCESS;
}

void nsTestBaseClass::DoTestDeInitialization()
{
  try

  {
    if (DeInitializeTest() == NS_FAILURE)
      nsTestFramework::Output(nsTestOutput::Error, "Test DeInitialization failed.");
  }
  catch (...)
  {
    nsTestFramework::Output(nsTestOutput::Error, "Exception during test de-initialization.");
  }
}

nsResult nsTestBaseClass::DoSubTestInitialization(nsInt32 iIdentifier)
{
  try
  {
    if (InitializeSubTest(iIdentifier) == NS_FAILURE)
    {
      nsTestFramework::Output(nsTestOutput::Error, "Sub-Test Initialization failed, skipping Test.");
      return NS_FAILURE;
    }
  }
  catch (...)
  {
    nsTestFramework::Output(nsTestOutput::Error, "Exception during sub-test initialization.");
    return NS_FAILURE;
  }

  return NS_SUCCESS;
}

void nsTestBaseClass::DoSubTestDeInitialization(nsInt32 iIdentifier)
{
  try
  {
    if (DeInitializeSubTest(iIdentifier) == NS_FAILURE)
      nsTestFramework::Output(nsTestOutput::Error, "Sub-Test De-Initialization failed.");
  }
  catch (...)
  {
    nsTestFramework::Output(nsTestOutput::Error, "Exception during sub-test de-initialization.");
  }
}

nsTestAppRun nsTestBaseClass::DoSubTestRun(nsInt32 iIdentifier, double& fDuration, nsUInt32 uiInvocationCount)
{
  fDuration = 0.0;

  nsTestAppRun ret = nsTestAppRun::Quit;

  try
  {
    nsTime StartTime = nsTime::Now();

    ret = RunSubTest(iIdentifier, uiInvocationCount);

    fDuration = (nsTime::Now() - StartTime).GetMilliseconds();
  }
  catch (...)
  {
    nsInt32 iEntry = -1;

    for (nsInt32 i = 0; i < (nsInt32)m_Entries.size(); ++i)
    {
      if (m_Entries[i].m_iIdentifier == iIdentifier)
      {
        iEntry = i;
        break;
      }
    }

    if (iEntry >= 0)
      nsTestFramework::Output(nsTestOutput::Error, "Exception during sub-test '%s'.", m_Entries[iEntry].m_szName);
    else
      nsTestFramework::Output(nsTestOutput::Error, "Exception during unknown sub-test.");
  }

  return ret;
}


NS_STATICLINK_FILE(TestFramework, TestFramework_Framework_TestBaseClass);
