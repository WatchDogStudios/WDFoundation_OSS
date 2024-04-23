/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <ToolsFoundationTest/ToolsFoundationTestPCH.h>

#include <TestFramework/Framework/TestFramework.h>
#include <TestFramework/Utilities/TestSetup.h>

nsInt32 nsConstructionCounter::s_iConstructions = 0;
nsInt32 nsConstructionCounter::s_iDestructions = 0;
nsInt32 nsConstructionCounter::s_iConstructionsLast = 0;
nsInt32 nsConstructionCounter::s_iDestructionsLast = 0;

NS_TESTFRAMEWORK_ENTRY_POINT("ToolsFoundationTest", "Tools Foundation Tests")
