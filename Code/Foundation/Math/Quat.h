#pragma once

#include <Foundation/Math/Vec3.h>

/// \brief Quaternions can be used to represent rotations in 3D space.
///
/// Quaternions are useful to represent 3D rotations, as they are smaller and more efficient than matrices
/// and can be concatenated easily, without having the 'Gimbal Lock' problem of Euler Angles.
/// Either use a full blown transformation (e.g. a 4x4 matrix) to represent a object, or use a Quaternion
/// bundled with a position vector, if (non-uniform) scale is not required.
/// Quaternions can also easily be interpolated (via Slerp).
/// This implementation also allows to convert back and forth between Quaternions and Matrices easily.
///
/// Quaternions have no 'IsIdentical' or 'IsEqual' function, as there can be different representations for the
/// same rotation, and it is rather difficult to check this. So to not convey any false notion of being equal
/// (or rather unequal), those functions are not provided.
template <typename Type>
class nsQuatTemplate
{
public:
  // Means this object can be copied using memcpy instead of copy construction.
  NS_DECLARE_POD_TYPE();

  using ComponentType = Type;

  // *** Data ***
public:
  Type x;
  Type y;
  Type z;
  Type w;

  // *** Constructors ***
public:
  nsQuatTemplate(); // [tested]

  /// \brief For internal use. You should never construct quaternions this way.
  nsQuatTemplate(Type x, Type y, Type z, Type w); // [tested]

#if NS_ENABLED(NS_MATH_CHECK_FOR_NAN)
  void AssertNotNaN() const
  {
    NS_ASSERT_ALWAYS(!IsNaN(), "This object contains NaN values. This can happen when you forgot to initialize it before using it. Please check that "
                               "all code-paths properly initialize this object.");
  }
#endif

  /// \brief Static function that returns a quaternion that represents the identity rotation (none).
  [[nodiscard]] static const nsQuatTemplate<Type> MakeIdentity(); // [tested]

  // *** Functions to create a quaternion ***
public:
  /// \brief Sets the Quaternion to the identity.
  void SetIdentity(); // [tested]

  /// \brief Sets the individual elements of the quaternion directly. Note that x,y,z do NOT represent a rotation axis, and w does NOT represent an
  /// angle.
  ///
  /// Use this function only if you have good understanding of quaternion math and know exactly what you are doing.
  [[nodiscard]] static nsQuatTemplate<Type> MakeFromElements(Type x, Type y, Type z, Type w); // [tested]

  /// \brief Creates a quaternion from a rotation-axis and an angle.
  [[nodiscard]] static nsQuatTemplate<Type> MakeFromAxisAndAngle(const nsVec3Template<Type>& vRotationAxis, nsAngle angle); // [tested]

  /// \brief Creates a quaternion, that rotates through the shortest arc from "vDirFrom" to "vDirTo".
  [[nodiscard]] static nsQuatTemplate<Type> MakeShortestRotation(const nsVec3Template<Type>& vDirFrom, const nsVec3Template<Type>& vDirTo); // [tested]

  /// \brief Creates a quaternion from the given matrix.
  [[nodiscard]] static nsQuatTemplate<Type> MakeFromMat3(const nsMat3Template<Type>& m); // [tested]

  /// \brief Reconstructs a rotation quaternion from a matrix that may contain scaling and mirroring.
  ///
  /// In skeletal animation it is possible that matrices with mirroring are used, that need to be converted to a
  /// proper quaternion, even though a rotation with mirroring can't be represented by a quaternion.
  /// This function reconstructs a valid quaternion from such matrices. Obviously the mirroring information gets lost,
  /// but it is typically not needed any further anway.
  void ReconstructFromMat3(const nsMat3Template<Type>& m);

  /// \brief Reconstructs a rotation quaternion from a matrix that may contain scaling and mirroring.
  ///
  /// \sa ReconstructFromMat3()
  void ReconstructFromMat4(const nsMat4Template<Type>& m);

  /// \brief Returns a quaternion that is the spherical linear interpolation of the other two.
  [[nodiscard]] static nsQuatTemplate<Type> MakeSlerp(const nsQuatTemplate& qFrom, const nsQuatTemplate& qTo, Type t); // [tested]

  // *** Common Functions ***
public:
  /// \brief Normalizes the quaternion to unit length. ALL rotation-quaternions should be normalized at all times (automatically).
  void Normalize(); // [tested]

  /// \brief Returns the rotation-axis and angle, that this quaternion rotates around.
  void GetRotationAxisAndAngle(nsVec3Template<Type>& out_vAxis, nsAngle& out_angle, Type fEpsilon = nsMath::DefaultEpsilon<Type>()) const; // [tested]

  /// \brief Returns the x,y,z components as a vector.
  nsVec3Template<Type> GetVectorPart() const { return nsVec3Template<Type>(x, y, z); }

  /// \brief Returns the Quaternion as a matrix.
  const nsMat3Template<Type> GetAsMat3() const; // [tested]

  /// \brief Returns the Quaternion as a matrix.
  const nsMat4Template<Type> GetAsMat4() const; // [tested]

  /// \brief Checks whether all components are neither NaN nor infinite and that the quaternion is normalized.
  bool IsValid(Type fEpsilon = nsMath::DefaultEpsilon<Type>()) const; // [tested]

  /// \brief Checks whether any component is NaN.
  bool IsNaN() const; // [tested]

  /// \brief Determines whether \a this and \a qOther represent the same rotation. This is a rather slow operation.
  ///
  /// Currently it fails when one of the given quaternions is identity (so no rotation, at all), as it tries to
  /// compare rotation axis' and angles, which is undefined for the identity quaternion (also there are infinite
  /// representations for 'identity', so it's difficult to check for it).
  bool IsEqualRotation(const nsQuatTemplate& qOther, Type fEpsilon) const; // [tested]

  /// \brief Inverts the rotation, so instead of rotating N degrees around an axis, the quaternion will rotate -N degrees around its axis.
  ///
  /// This modifies the quaternion in place. If you want to get the inverse as a copy, use GetInverse().
  void Invert();

  /// \brief Returns a quaternion that represents the negative / inverted rotation. E.g. the one that would rotate back to identity.
  const nsQuatTemplate<Type> GetInverse() const; // [tested]

  /// \brief Returns the Quaternion with all 4 components negated. This is not the same as the inverted rotation!
  const nsQuatTemplate<Type> GetNegated() const;

  /// \brief Returns the dot-product of the two quaternions (commutative, order does not matter).
  Type Dot(const nsQuatTemplate& rhs) const; // [tested]

  // *** Euler Angle Conversions ***
public:
  /// \brief Converts the quaternion to Euler angles
  void GetAsEulerAngles(nsAngle& out_x, nsAngle& out_y, nsAngle& out_z) const; // [tested]

  /// \brief Sets the quaternion from Euler angles
  [[nodiscard]] static nsQuatTemplate<Type> MakeFromEulerAngles(const nsAngle& x, const nsAngle& y, const nsAngle& z); // [tested]
};

/// \brief Rotates v by q
template <typename Type>
const nsVec3Template<Type> operator*(const nsQuatTemplate<Type>& q, const nsVec3Template<Type>& v); // [tested]

/// \brief Concatenates the rotations of q1 and q2
template <typename Type>
const nsQuatTemplate<Type> operator*(const nsQuatTemplate<Type>& q1, const nsQuatTemplate<Type>& q2); // [tested]

template <typename Type>
bool operator==(const nsQuatTemplate<Type>& q1, const nsQuatTemplate<Type>& q2);                      // [tested]

template <typename Type>
bool operator!=(const nsQuatTemplate<Type>& q1, const nsQuatTemplate<Type>& q2);                      // [tested]

#include <Foundation/Math/Implementation/Quat_inl.h>
