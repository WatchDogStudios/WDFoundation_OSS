/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/IO/StreamUtils.h>

void nsStreamUtils::ReadAllAndAppend(nsStreamReader& inout_stream, nsDynamicArray<nsUInt8>& ref_destination)
{
  nsUInt8 temp[1024 * 4];

  while (true)
  {
    const nsUInt32 uiRead = (nsUInt32)inout_stream.ReadBytes(temp, NS_ARRAY_SIZE(temp));

    if (uiRead == 0)
      return;

    ref_destination.PushBackRange(nsArrayPtr<nsUInt8>(temp, uiRead));
  }
}


NS_STATICLINK_FILE(Foundation, Foundation_IO_Implementation_StreamUtils);
