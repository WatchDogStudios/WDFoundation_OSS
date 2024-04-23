/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Configuration/CVar.h>

template <typename Type, nsCVarType::Enum CVarType>
nsTypedCVar<Type, CVarType>::nsTypedCVar(nsStringView sName, const Type& value, nsBitflags<nsCVarFlags> flags, nsStringView sDescription)
  : nsCVar(sName, flags, sDescription)
{
  NS_ASSERT_DEBUG(sName.FindSubString(" ") == nullptr, "CVar names must not contain whitespace");

  for (nsUInt32 i = 0; i < nsCVarValue::ENUM_COUNT; ++i)
    m_Values[i] = value;
}

template <typename Type, nsCVarType::Enum CVarType>
nsTypedCVar<Type, CVarType>::operator const Type&() const
{
  return (m_Values[nsCVarValue::Current]);
}

template <typename Type, nsCVarType::Enum CVarType>
nsCVarType::Enum nsTypedCVar<Type, CVarType>::GetType() const
{
  return CVarType;
}

template <typename Type, nsCVarType::Enum CVarType>
void nsTypedCVar<Type, CVarType>::SetToRestartValue()
{
  if (m_Values[nsCVarValue::Current] == m_Values[nsCVarValue::Restart])
    return;

  // this will NOT trigger a 'restart value changed' event
  m_Values[nsCVarValue::Current] = m_Values[nsCVarValue::Restart];

  nsCVarEvent e(this);
  e.m_EventType = nsCVarEvent::ValueChanged;
  m_CVarEvents.Broadcast(e);

  // broadcast the same to the 'all cvars' event handlers
  s_AllCVarEvents.Broadcast(e);
}

template <typename Type, nsCVarType::Enum CVarType>
const Type& nsTypedCVar<Type, CVarType>::GetValue(nsCVarValue::Enum val) const
{
  return (m_Values[val]);
}

template <typename Type, nsCVarType::Enum CVarType>
void nsTypedCVar<Type, CVarType>::operator=(const Type& value)
{
  nsCVarEvent e(this);

  if (GetFlags().IsAnySet(nsCVarFlags::RequiresRestart))
  {
    if (value == m_Values[nsCVarValue::Restart]) // no change
      return;

    e.m_EventType = nsCVarEvent::RestartValueChanged;
  }
  else
  {
    if (m_Values[nsCVarValue::Current] == value) // no change
      return;

    m_Values[nsCVarValue::Current] = value;
    e.m_EventType = nsCVarEvent::ValueChanged;
  }

  m_Values[nsCVarValue::Restart] = value;

  m_CVarEvents.Broadcast(e);

  // broadcast the same to the 'all cvars' event handlers
  s_AllCVarEvents.Broadcast(e);
}
