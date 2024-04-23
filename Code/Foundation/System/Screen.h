/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Logging/Log.h>
#include <Foundation/Math/Rect.h>
#include <Foundation/Math/Size.h>
#include <Foundation/Strings/String.h>

/// \brief Describes the properties of a screen
struct NS_FOUNDATION_DLL nsScreenInfo
{
  nsString m_sDisplayName; ///< Some OS provided name for the screen, typically the manufacturer and model name.

  nsInt32 m_iOffsetX;     ///< The virtual position of the screen. Ie. a window created at this location will appear on this screen.
  nsInt32 m_iOffsetY;     ///< The virtual position of the screen. Ie. a window created at this location will appear on this screen.
  nsInt32 m_iResolutionX; ///< The virtual resolution. Ie. a window with this dimension will span the entire screen.
  nsInt32 m_iResolutionY; ///< The virtual resolution. Ie. a window with this dimension will span the entire screen.
  bool m_bIsPrimary;      ///< Whether this is the primary/main screen.
};

/// \brief Provides functionality to detect available monitors
class NS_FOUNDATION_DLL nsScreen
{
public:
  /// \brief Enumerates all available screens. When it returns NS_SUCCESS, at least one screen has been found.
  static nsResult EnumerateScreens(nsHybridArray<nsScreenInfo, 2>& out_screens);

  /// \brief Prints the available screen information to the provided log.
  static void PrintScreenInfo(const nsHybridArray<nsScreenInfo, 2>& screens, nsLogInterface* pLog = nsLog::GetThreadLocalLogSystem());
};
