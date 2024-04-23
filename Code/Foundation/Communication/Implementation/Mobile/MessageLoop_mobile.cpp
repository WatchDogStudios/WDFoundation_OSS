/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#if NS_DISABLED(NS_PLATFORM_WINDOWS_DESKTOP)

#  include <Foundation/Communication/Implementation/Mobile/MessageLoop_mobile.h>
#  include <Foundation/Communication/IpcChannel.h>

nsMessageLoop_mobile::nsMessageLoop_mobile() {}

nsMessageLoop_mobile::~nsMessageLoop_mobile()
{
  StopUpdateThread();
}

void nsMessageLoop_mobile::WakeUp()
{
  // nothing to do
}

bool nsMessageLoop_mobile::WaitForMessages(nsInt32 iTimeout, nsIpcChannel* pFilter)
{
  // nothing to do

  if (iTimeout < 0)
  {
    // if timeout is 'indefinite' wait a little
    nsThreadUtils::YieldTimeSlice();
  }

  return false;
}

#endif



NS_STATICLINK_FILE(Foundation, Foundation_Communication_Implementation_Mobile_MessageLoop_mobile);
