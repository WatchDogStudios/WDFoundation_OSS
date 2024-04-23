/*
 *   Copyright (c) 2023 WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#if SHADING_QUALITY != SHADING_QUALITY_SIMPLIFIED
#error "Functions in LightDataSimplified.h are only for SIMPLIFIED shading quality. Include LightData.h instead."
#endif

#include "Platforms.h"
#include "ConstantBufferMacros.h"

CONSTANT_BUFFER(nsSimplifiedDataConstants, 3)
{
  UINT1(SkyIrradianceIndex);  
};
