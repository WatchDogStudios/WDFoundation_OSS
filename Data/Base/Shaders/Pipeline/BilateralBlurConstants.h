/*
 *   Copyright (c) 2023 WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include "../Common/Platforms.h"
#include "../Common/ConstantBufferMacros.h"

CONSTANT_BUFFER(nsBilateralBlurConstants, 3)
{
  UINT1(BlurRadius);
  FLOAT1(GaussianFalloff);  // 1 / (2 * sigma * sigma)
  FLOAT1(Sharpness); // 0 is aquivalent with a Gaussian blur
};

