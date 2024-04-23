/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <TestFramework/TestFrameworkPCH.h>

NS_STATICLINK_LIBRARY(TestFramework)
{
  if (bReturn)
    return;

  NS_STATICLINK_REFERENCE(TestFramework_Framework_Qt_qtLogMessageDock);
  NS_STATICLINK_REFERENCE(TestFramework_Framework_Qt_qtTestDelegate);
  NS_STATICLINK_REFERENCE(TestFramework_Framework_Qt_qtTestFramework);
  NS_STATICLINK_REFERENCE(TestFramework_Framework_Qt_qtTestGUI);
  NS_STATICLINK_REFERENCE(TestFramework_Framework_Qt_qtTestModel);
  NS_STATICLINK_REFERENCE(TestFramework_Framework_SimpleTest);
  NS_STATICLINK_REFERENCE(TestFramework_Framework_TestBaseClass);
  NS_STATICLINK_REFERENCE(TestFramework_Framework_TestFramework);
  NS_STATICLINK_REFERENCE(TestFramework_Framework_TestResults);
  NS_STATICLINK_REFERENCE(TestFramework_Framework_uwp_uwpTestApplication);
  NS_STATICLINK_REFERENCE(TestFramework_Framework_uwp_uwpTestFramework);
  NS_STATICLINK_REFERENCE(TestFramework_Utilities_TestLogInterface);
  NS_STATICLINK_REFERENCE(TestFramework_Utilities_TestOrder);
  NS_STATICLINK_REFERENCE(TestFramework_Utilities_TestSetup);
}
