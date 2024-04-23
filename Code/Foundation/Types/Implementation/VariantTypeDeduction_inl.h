/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */



/// \cond

template <>
struct nsVariantTypeDeduction<bool>
{
  enum
  {
    value = nsVariantType::Bool,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = bool;
  using ReturnType = bool;
};

template <>
struct nsVariantTypeDeduction<nsInt8>
{
  enum
  {
    value = nsVariantType::Int8,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsInt8;
};

template <>
struct nsVariantTypeDeduction<nsUInt8>
{
  enum
  {
    value = nsVariantType::UInt8,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsUInt8;
};

template <>
struct nsVariantTypeDeduction<nsInt16>
{
  enum
  {
    value = nsVariantType::Int16,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsInt16;
};

template <>
struct nsVariantTypeDeduction<nsUInt16>
{
  enum
  {
    value = nsVariantType::UInt16,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsUInt16;
};

template <>
struct nsVariantTypeDeduction<nsInt32>
{
  enum
  {
    value = nsVariantType::Int32,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsInt32;
};

template <>
struct nsVariantTypeDeduction<nsUInt32>
{
  enum
  {
    value = nsVariantType::UInt32,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsUInt32;
};

template <>
struct nsVariantTypeDeduction<nsInt64>
{
  enum
  {
    value = nsVariantType::Int64,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsInt64;
};

template <>
struct nsVariantTypeDeduction<nsUInt64>
{
  enum
  {
    value = nsVariantType::UInt64,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsUInt64;
};

template <>
struct nsVariantTypeDeduction<float>
{
  enum
  {
    value = nsVariantType::Float,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = float;
};

template <>
struct nsVariantTypeDeduction<double>
{
  enum
  {
    value = nsVariantType::Double,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = double;
};

template <>
struct nsVariantTypeDeduction<nsColor>
{
  enum
  {
    value = nsVariantType::Color,
    forceSharing = false,
    hasReflectedMembers = true,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsColor;
};

template <>
struct nsVariantTypeDeduction<nsColorGammaUB>
{
  enum
  {
    value = nsVariantType::ColorGamma,
    forceSharing = false,
    hasReflectedMembers = true,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsColorGammaUB;
};

template <>
struct nsVariantTypeDeduction<nsVec2>
{
  enum
  {
    value = nsVariantType::Vector2,
    forceSharing = false,
    hasReflectedMembers = true,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsVec2;
};

template <>
struct nsVariantTypeDeduction<nsVec3>
{
  enum
  {
    value = nsVariantType::Vector3,
    forceSharing = false,
    hasReflectedMembers = true,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsVec3;
};

template <>
struct nsVariantTypeDeduction<nsVec4>
{
  enum
  {
    value = nsVariantType::Vector4,
    forceSharing = false,
    hasReflectedMembers = true,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsVec4;
};

template <>
struct nsVariantTypeDeduction<nsVec2I32>
{
  enum
  {
    value = nsVariantType::Vector2I,
    forceSharing = false,
    hasReflectedMembers = true,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsVec2I32;
};

template <>
struct nsVariantTypeDeduction<nsVec3I32>
{
  enum
  {
    value = nsVariantType::Vector3I,
    forceSharing = false,
    hasReflectedMembers = true,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsVec3I32;
};

template <>
struct nsVariantTypeDeduction<nsVec4I32>
{
  enum
  {
    value = nsVariantType::Vector4I,
    forceSharing = false,
    hasReflectedMembers = true,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsVec4I32;
};

template <>
struct nsVariantTypeDeduction<nsVec2U32>
{
  enum
  {
    value = nsVariantType::Vector2U,
    forceSharing = false,
    hasReflectedMembers = true,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsVec2U32;
};

template <>
struct nsVariantTypeDeduction<nsVec3U32>
{
  enum
  {
    value = nsVariantType::Vector3U,
    forceSharing = false,
    hasReflectedMembers = true,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsVec3U32;
};

template <>
struct nsVariantTypeDeduction<nsVec4U32>
{
  enum
  {
    value = nsVariantType::Vector4U,
    forceSharing = false,
    hasReflectedMembers = true,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsVec4U32;
};

template <>
struct nsVariantTypeDeduction<nsQuat>
{
  enum
  {
    value = nsVariantType::Quaternion,
    forceSharing = false,
    hasReflectedMembers = true,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsQuat;
};

template <>
struct nsVariantTypeDeduction<nsMat3>
{
  enum
  {
    value = nsVariantType::Matrix3,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsMat3;
};

template <>
struct nsVariantTypeDeduction<nsMat4>
{
  enum
  {
    value = nsVariantType::Matrix4,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsMat4;
};

template <>
struct nsVariantTypeDeduction<nsTransform>
{
  enum
  {
    value = nsVariantType::Transform,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsTransform;
};

template <>
struct nsVariantTypeDeduction<nsString>
{
  enum
  {
    value = nsVariantType::String,
    forceSharing = true,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsString;
};

template <>
struct nsVariantTypeDeduction<nsUntrackedString>
{
  enum
  {
    value = nsVariantType::String,
    forceSharing = true,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsString;
};

template <>
struct nsVariantTypeDeduction<nsStringView>
{
  enum
  {
    value = nsVariantType::StringView,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsStringView;
};

template <>
struct nsVariantTypeDeduction<nsDataBuffer>
{
  enum
  {
    value = nsVariantType::DataBuffer,
    forceSharing = true,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsDataBuffer;
};

template <>
struct nsVariantTypeDeduction<char*>
{
  enum
  {
    value = nsVariantType::String,
    forceSharing = true,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsString;
};

template <>
struct nsVariantTypeDeduction<const char*>
{
  enum
  {
    value = nsVariantType::String,
    forceSharing = true,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsString;
};

template <size_t N>
struct nsVariantTypeDeduction<char[N]>
{
  enum
  {
    value = nsVariantType::String,
    forceSharing = true,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsString;
};

template <size_t N>
struct nsVariantTypeDeduction<const char[N]>
{
  enum
  {
    value = nsVariantType::String,
    forceSharing = true,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsString;
};

template <>
struct nsVariantTypeDeduction<nsTime>
{
  enum
  {
    value = nsVariantType::Time,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsTime;
};

template <>
struct nsVariantTypeDeduction<nsUuid>
{
  enum
  {
    value = nsVariantType::Uuid,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsUuid;
};

template <>
struct nsVariantTypeDeduction<nsAngle>
{
  enum
  {
    value = nsVariantType::Angle,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsAngle;
};

template <>
struct nsVariantTypeDeduction<nsHashedString>
{
  enum
  {
    value = nsVariantType::HashedString,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsHashedString;
};

template <>
struct nsVariantTypeDeduction<nsTempHashedString>
{
  enum
  {
    value = nsVariantType::TempHashedString,
    forceSharing = false,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsTempHashedString;
};

template <>
struct nsVariantTypeDeduction<nsVariantArray>
{
  enum
  {
    value = nsVariantType::VariantArray,
    forceSharing = true,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsVariantArray;
};

template <>
struct nsVariantTypeDeduction<nsArrayPtr<nsVariant>>
{
  enum
  {
    value = nsVariantType::VariantArray,
    forceSharing = true,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsVariantArray;
};


template <>
struct nsVariantTypeDeduction<nsVariantDictionary>
{
  enum
  {
    value = nsVariantType::VariantDictionary,
    forceSharing = true,
    hasReflectedMembers = false,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsVariantDictionary;
};

namespace nsInternal
{
  template <int v>
  struct PointerDeductionHelper
  {
  };

  template <>
  struct PointerDeductionHelper<0>
  {
    using StorageType = void*;
  };

  template <>
  struct PointerDeductionHelper<1>
  {
    using StorageType = nsReflectedClass*;
  };
} // namespace nsInternal

template <>
struct nsVariantTypeDeduction<nsTypedPointer>
{
  enum
  {
    value = nsVariantType::TypedPointer,
    forceSharing = false,
    hasReflectedMembers = true,
    classification = nsVariantClass::DirectCast
  };

  using StorageType = nsTypedPointer;
};

template <typename T>
struct nsVariantTypeDeduction<T*>
{
  enum
  {
    value = nsVariantType::TypedPointer,
    forceSharing = false,
    hasReflectedMembers = true,
    classification = nsVariantClass::PointerCast
  };

  using StorageType = nsTypedPointer;
};

template <>
struct nsVariantTypeDeduction<nsTypedObject>
{
  enum
  {
    value = nsVariantType::TypedObject,
    forceSharing = false,
    hasReflectedMembers = true,
    classification = nsVariantClass::TypedObject
  };

  using StorageType = nsTypedObject;
};

/// \endcond
