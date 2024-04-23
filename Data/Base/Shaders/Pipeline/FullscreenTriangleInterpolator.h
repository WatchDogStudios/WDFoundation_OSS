/*
 *   Copyright (c) 2023 WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Shaders/Common/GlobalConstants.h>

struct VS_OUT
{
  float4 Position : SV_Position;
  float2 TexCoord0 : TEXCOORD0;
#if CAMERA_MODE == CAMERA_MODE_STEREO
#  if defined(VERTEX_SHADER_RENDER_TARGET_ARRAY_INDEX)
  uint RenderTargetArrayIndex : SV_RenderTargetArrayIndex;
#  else
  uint RenderTargetArrayIndex : RENDERTARGETARRAYINDEX;
#  endif
#endif
};

typedef VS_OUT PS_IN;
