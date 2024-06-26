/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Math/Color.h>
#include <Foundation/Math/Math.h>

/// \brief A 8bit per channel color storage format with undefined encoding. It is up to the user to reinterpret as a gamma or linear space
/// color.
///
/// \see nsColorLinearUB
/// \see nsColorGammaUB
class NS_FOUNDATION_DLL nsColorBaseUB
{
public:
  NS_DECLARE_POD_TYPE();

  nsUInt8 r;
  nsUInt8 g;
  nsUInt8 b;
  nsUInt8 a;

  /// \brief Default-constructed color is uninitialized (for speed)
  nsColorBaseUB() = default;

  /// \brief Initializes the color with r, g, b, a
  nsColorBaseUB(nsUInt8 r, nsUInt8 g, nsUInt8 b, nsUInt8 a = 255);

  /// \brief Conversion to const nsUInt8*.
  const nsUInt8* GetData() const { return &r; }

  /// \brief Conversion to nsUInt8*
  nsUInt8* GetData() { return &r; }
};

NS_CHECK_AT_COMPILETIME(sizeof(nsColorBaseUB) == 4);

/// \brief A 8bit per channel unsigned normalized (values interpreted as 0-1) color storage format that represents colors in linear space.
///
/// For any calculations or conversions use nsColor.
/// \see nsColor
class NS_FOUNDATION_DLL nsColorLinearUB : public nsColorBaseUB
{
public:
  NS_DECLARE_POD_TYPE();

  /// \brief Default-constructed color is uninitialized (for speed)
  nsColorLinearUB() = default; // [tested]

  /// \brief Initializes the color with r, g, b, a
  nsColorLinearUB(nsUInt8 r, nsUInt8 g, nsUInt8 b, nsUInt8 a = 255); // [tested]

  /// \brief Initializes the color with nsColor.
  /// Assumes that the given color is normalized.
  /// \see nsColor::IsNormalized
  nsColorLinearUB(const nsColor& color); // [tested]

  /// \brief Initializes the color with nsColor.
  void operator=(const nsColor& color); // [tested]

  /// \brief Converts this color to nsColor.
  nsColor ToLinearFloat() const; // [tested]
};

NS_CHECK_AT_COMPILETIME(sizeof(nsColorLinearUB) == 4);

/// \brief A 8bit per channel unsigned normalized (values interpreted as 0-1) color storage format that represents colors in gamma space.
///
/// For any calculations or conversions use nsColor.
/// \see nsColor
class NS_FOUNDATION_DLL nsColorGammaUB : public nsColorBaseUB
{
public:
  NS_DECLARE_POD_TYPE();

  /// \brief Default-constructed color is uninitialized (for speed)
  nsColorGammaUB() = default;

  /// \brief Copies the color values. RGB are assumed to be in Gamma space.
  nsColorGammaUB(nsUInt8 uiGammaRed, nsUInt8 uiGammaGreen, nsUInt8 uiGammaBlue, nsUInt8 uiLinearAlpha = 255); // [tested]

  /// \brief Initializes the color with nsColor. Converts the linear space color to gamma space.
  /// Assumes that the given color is normalized.
  /// \see nsColor::IsNormalized
  nsColorGammaUB(const nsColor& color); // [tested]

  /// \brief Initializes the color with nsColor. Converts the linear space color to gamma space.
  void operator=(const nsColor& color); // [tested]

  /// \brief Converts this color to nsColor.
  nsColor ToLinearFloat() const;
};

NS_CHECK_AT_COMPILETIME(sizeof(nsColorGammaUB) == 4);


#include <Foundation/Math/Implementation/Color8UNorm_inl.h>
