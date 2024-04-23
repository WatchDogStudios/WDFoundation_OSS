/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Basics.h>

class NS_FOUNDATION_DLL nsProfilingUtils
{
public:
  /// \brief Captures profiling data via nsProfilingSystem::Capture and saves it to the giben file location.
  static nsResult SaveProfilingCapture(nsStringView sCapturePath);
  /// \brief Reads two profiling captures and merges them into one.
  static nsResult MergeProfilingCaptures(nsStringView sCapturePath1, nsStringView sCapturePath2, nsStringView sMergedCapturePath);
};
