/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */

#pragma once

#include <Foundation/Basics.h>
#include <Foundation/IO/Stream.h>

namespace nsStreamUtils
{
  /// \brief Reads all the remaining data in \a stream and appends it to \a destination.
  NS_FOUNDATION_DLL void ReadAllAndAppend(nsStreamReader& inout_stream, nsDynamicArray<nsUInt8>& ref_destination);

} // namespace nsStreamUtils
