/*
 *   Copyright (c) 2023 WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Shaders/Common/ConstantBufferMacros.h>

struct NS_SHADER_STRUCT nsOceanSimulationConstantBufferImmutable
{
  UINT1(Dim);
  UINT1(InputWidth);
  UINT1(OutputWidth);
  UINT1(OutputHeight);
  UINT1(XAddressOffset);
  UINT1(YAddressOffset);
};

struct NS_SHADER_STRUCT nsOceanSimulationConstantBufferPerFrame
{
  FLOAT1(TimeScale);
  FLOAT1(ChopScale);
  FLOAT1(GridLength);
  FLOAT1(SimPadding);
};

struct NS_SHADER_STRUCT nsOceanSimulationContstantBufferRender
{
  COLOR4F(WaterColor);
  FLOAT4(WaterSSRParams);
  FLOAT1(WaterTexelLength);
  FLOAT1(WaterPatchSize);
  FLOAT1(WaterHalfTexel);
  FLOAT1(WaterHeight);
  FLOAT1(WaterSurfDisplacementTol);
  FLOAT1(WaterPadding1);
  FLOAT1(WaterPadding2);
  FLOAT1(WaterPadding3);
};
