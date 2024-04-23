/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Configuration/Singleton.h>

nsMap<size_t, nsSingletonRegistry::SingletonEntry> nsSingletonRegistry::s_Singletons;

const nsMap<size_t, nsSingletonRegistry::SingletonEntry>& nsSingletonRegistry::GetAllRegisteredSingletons()
{
  return s_Singletons;
}

NS_STATICLINK_FILE(Foundation, Foundation_Configuration_Implementation_Singleton);
