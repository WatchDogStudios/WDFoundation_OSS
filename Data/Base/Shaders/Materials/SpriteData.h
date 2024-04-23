/*
 *   Copyright (c) 2023 WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Shaders/Common/ConstantBufferMacros.h>

struct NS_SHADER_STRUCT nsPerSpriteData
{
  FLOAT3(WorldSpacePosition);
  FLOAT1(Size);
  FLOAT1(MaxScreenSize);
  FLOAT1(AspectRatio);
  UINT1(ColorRG);
  UINT1(ColorBA);
  UINT1(TexCoordScale);
  UINT1(TexCoordOffset);
  UINT1(GameObjectID);
  UINT1(Reserved);
};
