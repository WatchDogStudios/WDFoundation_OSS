/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/SimdMath/SimdVec4i.h>

struct nsSimdMath
{
  static nsSimdVec4f Exp(const nsSimdVec4f& f);
  static nsSimdVec4f Ln(const nsSimdVec4f& f);
  static nsSimdVec4f Log2(const nsSimdVec4f& f);
  static nsSimdVec4i Log2i(const nsSimdVec4i& i);
  static nsSimdVec4f Log10(const nsSimdVec4f& f);
  static nsSimdVec4f Pow2(const nsSimdVec4f& f);

  static nsSimdVec4f Sin(const nsSimdVec4f& f);
  static nsSimdVec4f Cos(const nsSimdVec4f& f);
  static nsSimdVec4f Tan(const nsSimdVec4f& f);

  static nsSimdVec4f ASin(const nsSimdVec4f& f);
  static nsSimdVec4f ACos(const nsSimdVec4f& f);
  static nsSimdVec4f ATan(const nsSimdVec4f& f);
};

#include <Foundation/SimdMath/Implementation/SimdMath_inl.h>
