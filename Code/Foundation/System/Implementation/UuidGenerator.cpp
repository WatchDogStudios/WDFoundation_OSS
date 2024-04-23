/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Types/Uuid.h>


// Include inline file
#if NS_ENABLED(NS_PLATFORM_WINDOWS)
#  include <Foundation/System/Implementation/Win/UuidGenerator_win.h>
#elif NS_ENABLED(NS_PLATFORM_OSX)
#  include <Foundation/System/Implementation/Posix/UuidGenerator_posix.h>
#elif NS_ENABLED(NS_PLATFORM_LINUX)
#  include <Foundation/System/Implementation/Posix/UuidGenerator_posix.h>
#elif NS_ENABLED(NS_PLATFORM_ANDROID)
#  include <Foundation/System/Implementation/Android/UuidGenerator_android.h>
#else
#  error "Uuid generation functions are not implemented on current platform"
#endif

nsUuid nsUuid::MakeStableUuidFromString(nsStringView sString)
{
  nsUuid NewUuid;
  NewUuid.m_uiLow = nsHashingUtils::xxHash64String(sString);
  NewUuid.m_uiHigh = nsHashingUtils::xxHash64String(sString, 0x7FFFFFFFFFFFFFE7u);

  return NewUuid;
}

nsUuid nsUuid::MakeStableUuidFromInt(nsInt64 iInt)
{
  nsUuid NewUuid;
  NewUuid.m_uiLow = nsHashingUtils::xxHash64(&iInt, sizeof(nsInt64));
  NewUuid.m_uiHigh = nsHashingUtils::xxHash64(&iInt, sizeof(nsInt64), 0x7FFFFFFFFFFFFFE7u);

  return NewUuid;
}

NS_STATICLINK_FILE(Foundation, Foundation_System_Implementation_UuidGenerator);
