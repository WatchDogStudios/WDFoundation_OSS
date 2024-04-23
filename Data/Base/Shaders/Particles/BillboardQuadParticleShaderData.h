/*
 *   Copyright (c) 2023 WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include "ParticleSystemConstants.h"
#include "BaseParticleShaderData.h"

struct NS_SHADER_STRUCT nsBillboardQuadParticleShaderData
{
  FLOAT3(Position);
  PACKEDHALF2(RotationOffset, RotationSpeed, RotationOffsetAndSpeed);
};

// this is only defined during shader compilation
#if NS_ENABLED(PLATFORM_SHADER)

StructuredBuffer<nsBillboardQuadParticleShaderData> particleBillboardQuadData;

#else // C++

NS_CHECK_AT_COMPILETIME(sizeof(nsBillboardQuadParticleShaderData) == 16);

#endif

