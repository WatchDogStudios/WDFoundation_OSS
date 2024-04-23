/*
 *   Copyright (c) 2023 WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include "../Common/Platforms.h"
#include "../Common/ConstantBufferMacros.h"

CONSTANT_BUFFER(nsSSAOConstants, 3)
{
  FLOAT2(TexCoordsScale);
  FLOAT2(FadeOutParams);

  FLOAT1(WorldRadius);
  FLOAT1(MaxScreenSpaceRadius);
  FLOAT1(Contrast);
  FLOAT1(Intensity);

  FLOAT1(PositionBias);
  FLOAT1(MipLevelScale);
  FLOAT1(DepthBlurScale);
  FLOAT1(Padding);
};
