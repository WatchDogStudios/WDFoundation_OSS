/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationInternal.h>
NS_FOUNDATION_INTERNAL_HEADER

#include <uuid/uuid.h>


NS_CHECK_AT_COMPILETIME(sizeof(nsUInt64) * 2 == sizeof(uuid_t));

nsUuid nsUuid::MakeUuid()
{
  uuid_t uuid;
  uuid_generate(uuid);

  nsUInt64* uiUuidData = reinterpret_cast<nsUInt64*>(uuid);

  return nsUuid(uiUuidData[1], uiUuidData[0]);
}
