/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#if NS_DISABLED(NS_PLATFORM_WINDOWS_DESKTOP)

#  include <Foundation/Basics.h>
#  include <Foundation/Communication/Implementation/MessageLoop.h>

class NS_FOUNDATION_DLL nsMessageLoop_mobile : public nsMessageLoop
{
public:
  nsMessageLoop_mobile();
  ~nsMessageLoop_mobile();

protected:
  virtual void WakeUp() override;
  virtual bool WaitForMessages(nsInt32 iTimeout, nsIpcChannel* pFilter) override;

private:
};

#endif
