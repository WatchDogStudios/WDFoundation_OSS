/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/System/PlatformFeatures.h>
#include <Foundation/System/Screen.h>

#if NS_ENABLED(NS_SUPPORTS_GLFW)
#  include <Foundation/System/Implementation/glfw/Screen_glfw.inl>
#elif NS_ENABLED(NS_PLATFORM_WINDOWS_DESKTOP)
#  include <Foundation/System/Implementation/Win/Screen_win32.inl>
#elif NS_ENABLED(NS_PLATFORM_WINDOWS_UWP)
#  include <Foundation/System/Implementation/uwp/Screen_uwp.inl>
#else

nsResult nsScreen::EnumerateScreens(nsHybridArray<nsScreenInfo, 2>& out_Screens)
{
  NS_ASSERT_NOT_IMPLEMENTED;
  return NS_FAILURE;
}

#endif

void nsScreen::PrintScreenInfo(const nsHybridArray<nsScreenInfo, 2>& screens, nsLogInterface* pLog /*= nsLog::GetThreadLocalLogSystem()*/)
{
  NS_LOG_BLOCK(pLog, "Screens");

  nsLog::Info(pLog, "Found {0} screens", screens.GetCount());

  for (const auto& screen : screens)
  {
    nsLog::Dev(pLog, "'{0}': Offset = ({1}, {2}), Resolution = ({3}, {4}){5}", screen.m_sDisplayName, screen.m_iOffsetX, screen.m_iOffsetY, screen.m_iResolutionX, screen.m_iResolutionY, screen.m_bIsPrimary ? " (primary)" : "");
  }
}


NS_STATICLINK_FILE(Foundation, Foundation_System_Implementation_Screen);
