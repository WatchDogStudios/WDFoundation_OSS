/*
 *   Copyright (c) 2023 WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include "ParticleSystemConstants.h"

struct NS_SHADER_STRUCT nsBaseParticleShaderData
{
  PACKEDCOLOR4H(Color);
  PACKEDHALF2(Life, Size, LifeAndSize); // Life: 1 to 0
  UINT1(Variation); // only lower 8 bit
};

// this is only defined during shader compilation
#if NS_ENABLED(PLATFORM_SHADER)

StructuredBuffer<nsBaseParticleShaderData> particleBaseData;

#else // C++

NS_CHECK_AT_COMPILETIME(sizeof(nsBaseParticleShaderData) == 16);

#endif

