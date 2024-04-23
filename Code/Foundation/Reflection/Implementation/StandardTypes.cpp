/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Math/Transform.h>
#include <Foundation/Reflection/Reflection.h>
#include <Foundation/Strings/HashedString.h>

// clang-format off
NS_BEGIN_STATIC_REFLECTED_TYPE(nsEnumBase, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsBitflagsBase, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_DYNAMIC_REFLECTED_TYPE(nsReflectedClass, 1, nsRTTINoAllocator)
NS_END_DYNAMIC_REFLECTED_TYPE;

// *********************************************
// ***** Standard POD Types for Properties *****

NS_BEGIN_STATIC_REFLECTED_TYPE(bool, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(float, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(double, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsInt8, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsUInt8, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsInt16, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsUInt16, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsInt32, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsUInt32, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsInt64, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsUInt64, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsConstCharPtr, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsTime, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_FUNCTIONS
  {
    NS_SCRIPT_FUNCTION_PROPERTY(MakeFromNanoseconds, In, "Nanoseconds")->AddFlags(nsPropertyFlags::Const),
    NS_SCRIPT_FUNCTION_PROPERTY(MakeFromMicroseconds, In, "Microseconds")->AddFlags(nsPropertyFlags::Const),
    NS_SCRIPT_FUNCTION_PROPERTY(MakeFromMilliseconds, In, "Milliseconds")->AddFlags(nsPropertyFlags::Const),
    NS_SCRIPT_FUNCTION_PROPERTY(MakeFromSeconds, In, "Seconds")->AddFlags(nsPropertyFlags::Const),
    NS_SCRIPT_FUNCTION_PROPERTY(MakeFromMinutes, In, "Minutes")->AddFlags(nsPropertyFlags::Const),
    NS_SCRIPT_FUNCTION_PROPERTY(MakeFromHours, In, "Hours")->AddFlags(nsPropertyFlags::Const),
    NS_SCRIPT_FUNCTION_PROPERTY(MakeZero)->AddFlags(nsPropertyFlags::Const),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsColor, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_PROPERTIES
  {
    NS_MEMBER_PROPERTY("r", r),
    NS_MEMBER_PROPERTY("g", g),
    NS_MEMBER_PROPERTY("b", b),
    NS_MEMBER_PROPERTY("a", a),
  }
  NS_END_PROPERTIES;
  NS_BEGIN_FUNCTIONS
  {
    NS_CONSTRUCTOR_PROPERTY(float, float, float),
    NS_CONSTRUCTOR_PROPERTY(float, float, float, float),
    NS_CONSTRUCTOR_PROPERTY(nsColorLinearUB),
    NS_CONSTRUCTOR_PROPERTY(nsColorGammaUB),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsColorBaseUB, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_PROPERTIES
  {
    NS_MEMBER_PROPERTY("r", r),
    NS_MEMBER_PROPERTY("g", g),
    NS_MEMBER_PROPERTY("b", b),
    NS_MEMBER_PROPERTY("a", a),
  }
  NS_END_PROPERTIES;
  NS_BEGIN_FUNCTIONS
  {
    NS_CONSTRUCTOR_PROPERTY(nsUInt8, nsUInt8, nsUInt8),
    NS_CONSTRUCTOR_PROPERTY(nsUInt8, nsUInt8, nsUInt8, nsUInt8),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsColorGammaUB, nsColorBaseUB, 1, nsRTTINoAllocator)
{
  NS_BEGIN_FUNCTIONS
  {
    NS_CONSTRUCTOR_PROPERTY(nsUInt8, nsUInt8, nsUInt8),
    NS_CONSTRUCTOR_PROPERTY(nsUInt8, nsUInt8, nsUInt8, nsUInt8),
    NS_CONSTRUCTOR_PROPERTY(const nsColor&),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsColorLinearUB, nsColorBaseUB, 1, nsRTTINoAllocator)
{
  NS_BEGIN_FUNCTIONS
  {
    NS_CONSTRUCTOR_PROPERTY(nsUInt8, nsUInt8, nsUInt8),
    NS_CONSTRUCTOR_PROPERTY(nsUInt8, nsUInt8, nsUInt8, nsUInt8),
    NS_CONSTRUCTOR_PROPERTY(const nsColor&),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsVec2, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_PROPERTIES
  {
    NS_MEMBER_PROPERTY("x", x),
    NS_MEMBER_PROPERTY("y", y),
  }
  NS_END_PROPERTIES;
  NS_BEGIN_FUNCTIONS
  {
    NS_CONSTRUCTOR_PROPERTY(float),
    NS_CONSTRUCTOR_PROPERTY(float, float),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsVec3, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_PROPERTIES
  {
    NS_MEMBER_PROPERTY("x", x),
    NS_MEMBER_PROPERTY("y", y),
    NS_MEMBER_PROPERTY("z", z),
  }
  NS_END_PROPERTIES;
  NS_BEGIN_FUNCTIONS
  {
    NS_CONSTRUCTOR_PROPERTY(float),
    NS_CONSTRUCTOR_PROPERTY(float, float, float),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsVec4, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_PROPERTIES
  {
    NS_MEMBER_PROPERTY("x", x),
    NS_MEMBER_PROPERTY("y", y),
    NS_MEMBER_PROPERTY("z", z),
    NS_MEMBER_PROPERTY("w", w),
  }
  NS_END_PROPERTIES;
  NS_BEGIN_FUNCTIONS
  {
    NS_CONSTRUCTOR_PROPERTY(float),
    NS_CONSTRUCTOR_PROPERTY(float, float, float, float),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsVec2I32, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_PROPERTIES
  {
    NS_MEMBER_PROPERTY("x", x),
    NS_MEMBER_PROPERTY("y", y),
  }
  NS_END_PROPERTIES;
  NS_BEGIN_FUNCTIONS
  {
    NS_CONSTRUCTOR_PROPERTY(nsInt32),
    NS_CONSTRUCTOR_PROPERTY(nsInt32, nsInt32),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsVec3I32, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_PROPERTIES
  {
    NS_MEMBER_PROPERTY("x", x),
    NS_MEMBER_PROPERTY("y", y),
    NS_MEMBER_PROPERTY("z", z),
  }
  NS_END_PROPERTIES;
  NS_BEGIN_FUNCTIONS
  {
    NS_CONSTRUCTOR_PROPERTY(nsInt32),
    NS_CONSTRUCTOR_PROPERTY(nsInt32, nsInt32, nsInt32),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsVec4I32, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_PROPERTIES
  {
    NS_MEMBER_PROPERTY("x", x),
    NS_MEMBER_PROPERTY("y", y),
    NS_MEMBER_PROPERTY("z", z),
    NS_MEMBER_PROPERTY("w", w),
  }
  NS_END_PROPERTIES;
  NS_BEGIN_FUNCTIONS
  {
    NS_CONSTRUCTOR_PROPERTY(nsInt32),
    NS_CONSTRUCTOR_PROPERTY(nsInt32, nsInt32, nsInt32, nsInt32),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsVec2U32, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_PROPERTIES
  {
    NS_MEMBER_PROPERTY("x", x),
    NS_MEMBER_PROPERTY("y", y),
  }
  NS_END_PROPERTIES;
  NS_BEGIN_FUNCTIONS
  {
    NS_CONSTRUCTOR_PROPERTY(nsUInt32),
    NS_CONSTRUCTOR_PROPERTY(nsUInt32, nsUInt32),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsVec3U32, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_PROPERTIES
  {
    NS_MEMBER_PROPERTY("x", x),
    NS_MEMBER_PROPERTY("y", y),
    NS_MEMBER_PROPERTY("z", z),
  }
  NS_END_PROPERTIES;
  NS_BEGIN_FUNCTIONS
  {
    NS_CONSTRUCTOR_PROPERTY(nsUInt32),
    NS_CONSTRUCTOR_PROPERTY(nsUInt32, nsUInt32, nsUInt32),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsVec4U32, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_PROPERTIES
  {
    NS_MEMBER_PROPERTY("x", x),
    NS_MEMBER_PROPERTY("y", y),
    NS_MEMBER_PROPERTY("z", z),
    NS_MEMBER_PROPERTY("w", w),
  }
  NS_END_PROPERTIES;
  NS_BEGIN_FUNCTIONS
  {
    NS_CONSTRUCTOR_PROPERTY(nsUInt32),
    NS_CONSTRUCTOR_PROPERTY(nsUInt32, nsUInt32, nsUInt32, nsUInt32),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsQuat, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_PROPERTIES
  {
    NS_MEMBER_PROPERTY("x", x),
    NS_MEMBER_PROPERTY("y", y),
    NS_MEMBER_PROPERTY("z", z),
    NS_MEMBER_PROPERTY("w", w),
  }
  NS_END_PROPERTIES;
  NS_BEGIN_FUNCTIONS
  {
    NS_CONSTRUCTOR_PROPERTY(float, float, float, float),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsMat3, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsMat4, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsTransform, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_PROPERTIES
  {
    NS_MEMBER_PROPERTY("Position", m_vPosition),
    NS_MEMBER_PROPERTY("Rotation", m_qRotation),
    NS_MEMBER_PROPERTY("Scale", m_vScale),
  }
  NS_END_PROPERTIES;
  NS_BEGIN_FUNCTIONS
  {
    NS_CONSTRUCTOR_PROPERTY(nsVec3, nsQuat),
    NS_CONSTRUCTOR_PROPERTY(nsVec3, nsQuat, nsVec3),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_ENUM(nsBasisAxis, 1)
NS_ENUM_CONSTANT(nsBasisAxis::PositiveX),
NS_ENUM_CONSTANT(nsBasisAxis::PositiveY),
NS_ENUM_CONSTANT(nsBasisAxis::Positivns),
NS_ENUM_CONSTANT(nsBasisAxis::NegativeX),
NS_ENUM_CONSTANT(nsBasisAxis::NegativeY),
NS_ENUM_CONSTANT(nsBasisAxis::Negativns),
NS_END_STATIC_REFLECTED_ENUM;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsUuid, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsVariant, nsNoBase, 3, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsVariantArray, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsVariantDictionary, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsString, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsUntrackedString, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsStringView, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsDataBuffer, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsHashedString, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsTempHashedString, nsNoBase, 1, nsRTTINoAllocator)
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsAngle, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_FUNCTIONS
  {
    NS_FUNCTION_PROPERTY(MakeFromDegree),
    NS_FUNCTION_PROPERTY(MakeFromRadian),
  }
  NS_END_FUNCTIONS;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsFloatInterval, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_PROPERTIES
  {
    NS_MEMBER_PROPERTY("Start", m_StartValue),
    NS_MEMBER_PROPERTY("End", m_EndValue),
  }
  NS_END_PROPERTIES;
}
NS_END_STATIC_REFLECTED_TYPE;

NS_BEGIN_STATIC_REFLECTED_TYPE(nsIntInterval, nsNoBase, 1, nsRTTINoAllocator)
{
  NS_BEGIN_PROPERTIES
  {
    NS_MEMBER_PROPERTY("Start", m_StartValue),
    NS_MEMBER_PROPERTY("End", m_EndValue),
  }
  NS_END_PROPERTIES;
}
NS_END_STATIC_REFLECTED_TYPE;

// **********************************************************************
// ***** Various RTTI infos that can't be put next to their classes *****

NS_BEGIN_STATIC_REFLECTED_BITFLAGS(nsTypeFlags, 1)
NS_BITFLAGS_CONSTANTS(nsTypeFlags::StandardType, nsTypeFlags::IsEnum, nsTypeFlags::Bitflags, nsTypeFlags::Class, nsTypeFlags::Abstract, nsTypeFlags::Phantom, nsTypeFlags::Minimal)
NS_END_STATIC_REFLECTED_BITFLAGS;

NS_BEGIN_STATIC_REFLECTED_BITFLAGS(nsPropertyFlags, 1)
NS_BITFLAGS_CONSTANTS(nsPropertyFlags::StandardType, nsPropertyFlags::IsEnum, nsPropertyFlags::Bitflags, nsPropertyFlags::Class)
NS_BITFLAGS_CONSTANTS(nsPropertyFlags::Const, nsPropertyFlags::Reference, nsPropertyFlags::Pointer)
NS_BITFLAGS_CONSTANTS(nsPropertyFlags::PointerOwner, nsPropertyFlags::ReadOnly, nsPropertyFlags::Hidden, nsPropertyFlags::Phantom)
NS_END_STATIC_REFLECTED_BITFLAGS;

NS_BEGIN_STATIC_REFLECTED_ENUM(nsFunctionType, 1)
NS_ENUM_CONSTANTS(nsFunctionType::Member, nsFunctionType::StaticMember, nsFunctionType::Constructor)
NS_END_STATIC_REFLECTED_ENUM;

NS_BEGIN_STATIC_REFLECTED_ENUM(nsVariantType, 1)
NS_ENUM_CONSTANTS(nsVariantType::Invalid, nsVariantType::Bool, nsVariantType::Int8, nsVariantType::UInt8, nsVariantType::Int16, nsVariantType::UInt16)
NS_ENUM_CONSTANTS(nsVariantType::Int32, nsVariantType::UInt32, nsVariantType::Int64, nsVariantType::UInt64, nsVariantType::Float, nsVariantType::Double)
NS_ENUM_CONSTANTS(nsVariantType::Color, nsVariantType::Vector2, nsVariantType::Vector3, nsVariantType::Vector4)
NS_ENUM_CONSTANTS(nsVariantType::Vector2I, nsVariantType::Vector3I, nsVariantType::Vector4I, nsVariantType::Vector2U, nsVariantType::Vector3U, nsVariantType::Vector4U)
NS_ENUM_CONSTANTS(nsVariantType::Quaternion, nsVariantType::Matrix3, nsVariantType::Matrix4, nsVariantType::Transform)
NS_ENUM_CONSTANTS(nsVariantType::String, nsVariantType::StringView, nsVariantType::DataBuffer, nsVariantType::Time, nsVariantType::Uuid, nsVariantType::Angle, nsVariantType::ColorGamma)
NS_ENUM_CONSTANTS(nsVariantType::HashedString, nsVariantType::TempHashedString)
NS_ENUM_CONSTANTS(nsVariantType::VariantArray, nsVariantType::VariantDictionary, nsVariantType::TypedPointer, nsVariantType::TypedObject)
NS_END_STATIC_REFLECTED_ENUM;

NS_BEGIN_STATIC_REFLECTED_ENUM(nsPropertyCategory, 1)
NS_ENUM_CONSTANTS(nsPropertyCategory::Constant, nsPropertyCategory::Member, nsPropertyCategory::Function, nsPropertyCategory::Array, nsPropertyCategory::Set, nsPropertyCategory::Map)
NS_END_STATIC_REFLECTED_ENUM;
// clang-format on

NS_STATICLINK_FILE(Foundation, Foundation_Reflection_Implementation_StandardTypes);
