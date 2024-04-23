/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Basics.h>
#include <Foundation/Containers/DynamicArray.h>

///\brief The compression method to be used
enum class nsCompressionMethod : nsUInt16
{
  ZStd = 0 ///< Only available when ZStd support is enabled in the build (default)
};

/// \brief This namespace contains utilities which can be used to compress and decompress data.
namespace nsCompressionUtils
{
  ///\brief Compresses the given data using the compression method eMethod into the dynamic array given in out_Data.
  NS_FOUNDATION_DLL nsResult Compress(nsArrayPtr<const nsUInt8> uncompressedData, nsCompressionMethod method, nsDynamicArray<nsUInt8>& out_data);

  ///\brief Decompresses the given data using the compression method eMethod into the dynamic array given in out_Data.
  NS_FOUNDATION_DLL nsResult Decompress(nsArrayPtr<const nsUInt8> compressedData, nsCompressionMethod method, nsDynamicArray<nsUInt8>& out_data);
} // namespace nsCompressionUtils
