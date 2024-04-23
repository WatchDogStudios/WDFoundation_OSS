/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

#include <Foundation/Strings/StringBuilder.h>
#include <Foundation/Strings/StringConversion.h>

/// \brief Converts an absolute path to a 'DOS device path'
///
/// https://docs.microsoft.com/dotnet/standard/io/file-path-formats#dos-device-paths
///
/// This is necessary to support very long file paths, ie. more than 260 characters.
class nsDosDevicePath
{
public:
  nsDosDevicePath(nsStringView sPath)
  {
    nsStringBuilder tmp("\\\\?\\", sPath);
    tmp.ReplaceAll("/", "\\");
    m_Data = tmp.GetData();
  }

  const wchar_t* GetData() const { return m_Data.GetData(); }

  operator const wchar_t*() const { return m_Data.GetData(); }

  nsStringWChar m_Data;
};
