/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Serialization/AbstractObjectGraph.h>
#include <Foundation/Serialization/GraphPatch.h>
#include <Foundation/Serialization/GraphVersioning.h>
#include <Foundation/Serialization/RttiConverter.h>

NS_ENUMERABLE_CLASS_IMPLEMENTATION(nsGraphPatch);

nsGraphPatch::nsGraphPatch(const char* szType, nsUInt32 uiTypeVersion, PatchType type)
  : m_szType(szType)
  , m_uiTypeVersion(uiTypeVersion)
  , m_PatchType(type)
{
}

const char* nsGraphPatch::GetType() const
{
  return m_szType;
}

nsUInt32 nsGraphPatch::GetTypeVersion() const
{
  return m_uiTypeVersion;
}


nsGraphPatch::PatchType nsGraphPatch::GetPatchType() const
{
  return m_PatchType;
}

NS_STATICLINK_FILE(Foundation, Foundation_Serialization_Implementation_GraphPatch);
