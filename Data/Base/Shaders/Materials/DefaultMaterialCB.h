/*
 *   Copyright (c) 2023 WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Shaders/Common/GlobalConstants.h>

CONSTANT_BUFFER(nsMaterialConstants, 1)
{
  COLOR4F(BaseColor);
  COLOR4F(EmissiveColor);
  FLOAT1(MetallicValue);
  FLOAT1(ReflectanceValue);
  FLOAT1(RoughnessValue);
  FLOAT1(MaskThreshold);
  BOOL1(UseBaseTexture);
  BOOL1(UseNormalTexture);
  BOOL1(UseRoughnessTexture);
  BOOL1(UseMetallicTexture);
  BOOL1(UseEmissiveTexture);
  BOOL1(UseOcclusionTexture);
  BOOL1(UseOrmTexture);
};
