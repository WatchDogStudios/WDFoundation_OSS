/*
 *   Copyright (c) 2023 WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#if !defined(RENDER_PASS) || !defined(BLEND_MODE)
  #error "RENDER_PASS and BLEND_MODE permutations must be defined"
#endif

#define USE_WORLDPOS

#if SHADING_QUALITY == SHADING_QUALITY_NORMAL
  #include <Shaders/Materials/MaterialPixelShaderNormal.h>
#elif SHADING_QUALITY == SHADING_QUALITY_SIMPLIFIED
  #include <Shaders/Materials/MaterialPixelShaderSimplified.h>
#else
  #error "Unknown shading quality configuration."
#endif