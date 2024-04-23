/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Math/Frustum.h>
#include <Foundation/SimdMath/SimdBBox.h>
#include <Foundation/SimdMath/SimdConversion.h>
#include <Foundation/SimdMath/SimdVec4f.h>
#include <Foundation/Utilities/GraphicsUtils.h>

nsFrustum::nsFrustum() = default;
nsFrustum::~nsFrustum() = default;

const nsPlane& nsFrustum::GetPlane(nsUInt8 uiPlane) const
{
  NS_ASSERT_DEBUG(uiPlane < PLANE_COUNT, "Invalid plane index.");

  return m_Planes[uiPlane];
}

nsPlane& nsFrustum::AccessPlane(nsUInt8 uiPlane)
{
  NS_ASSERT_DEBUG(uiPlane < PLANE_COUNT, "Invalid plane index.");

  return m_Planes[uiPlane];
}

bool nsFrustum::IsValid() const
{
  for (nsUInt32 i = 0; i < PLANE_COUNT; ++i)
  {
    if (!m_Planes[i].IsValid())
      return false;
  }

  return true;
}

nsFrustum nsFrustum::MakeFromPlanes(const nsPlane* pPlanes)
{
  nsFrustum f;

  for (nsUInt32 i = 0; i < PLANE_COUNT; ++i)
    f.m_Planes[i] = pPlanes[i];

  return f;
}

void nsFrustum::TransformFrustum(const nsMat4& mTransform)
{
  for (nsUInt32 i = 0; i < PLANE_COUNT; ++i)
    m_Planes[i].Transform(mTransform);
}

nsVolumePosition::Enum nsFrustum::GetObjectPosition(const nsVec3* pVertices, nsUInt32 uiNumVertices) const
{
  /// \test Not yet tested

  bool bOnSomePlane = false;

  for (nsUInt32 i = 0; i < PLANE_COUNT; ++i)
  {
    const nsPositionOnPlane::Enum pos = m_Planes[i].GetObjectPosition(pVertices, uiNumVertices);

    if (pos == nsPositionOnPlane::Back)
      continue;

    if (pos == nsPositionOnPlane::Front)
      return nsVolumePosition::Outside;

    bOnSomePlane = true;
  }

  if (bOnSomePlane)
    return nsVolumePosition::Intersecting;

  return nsVolumePosition::Inside;
}

static nsPositionOnPlane::Enum GetPlaneObjectPosition(const nsPlane& p, const nsVec3* const pPoints, nsUInt32 uiVertices, const nsMat4& mTransform)
{
  bool bFront = false;
  bool bBack = false;

  for (nsUInt32 i = 0; i < uiVertices; ++i)
  {
    switch (p.GetPointPosition(mTransform * pPoints[i]))
    {
      case nsPositionOnPlane::Front:
      {
        if (bBack)
          return nsPositionOnPlane::Spanning;

        bFront = true;
      }
      break;

      case nsPositionOnPlane::Back:
      {
        if (bFront)
          return (nsPositionOnPlane::Spanning);

        bBack = true;
      }
      break;

      default:
        break;
    }
  }

  return (bFront ? nsPositionOnPlane::Front : nsPositionOnPlane::Back);
}


nsVolumePosition::Enum nsFrustum::GetObjectPosition(const nsVec3* pVertices, nsUInt32 uiNumVertices, const nsMat4& mObjectTransform) const
{
  /// \test Not yet tested

  bool bOnSomePlane = false;

  for (nsUInt32 i = 0; i < PLANE_COUNT; ++i)
  {
    const nsPositionOnPlane::Enum pos = GetPlaneObjectPosition(m_Planes[i], pVertices, uiNumVertices, mObjectTransform);

    if (pos == nsPositionOnPlane::Back)
      continue;

    if (pos == nsPositionOnPlane::Front)
      return nsVolumePosition::Outside;

    bOnSomePlane = true;
  }

  if (bOnSomePlane)
    return nsVolumePosition::Intersecting;

  return nsVolumePosition::Inside;
}

nsVolumePosition::Enum nsFrustum::GetObjectPosition(const nsBoundingSphere& sphere) const
{
  /// \test Not yet tested

  bool bOnSomePlane = false;

  for (nsUInt32 i = 0; i < PLANE_COUNT; ++i)
  {
    const nsPositionOnPlane::Enum pos = m_Planes[i].GetObjectPosition(sphere);

    if (pos == nsPositionOnPlane::Back)
      continue;

    if (pos == nsPositionOnPlane::Front)
      return nsVolumePosition::Outside;

    bOnSomePlane = true;
  }

  if (bOnSomePlane)
    return nsVolumePosition::Intersecting;

  return nsVolumePosition::Inside;
}

nsVolumePosition::Enum nsFrustum::GetObjectPosition(const nsBoundingBox& box) const
{
  /// \test Not yet tested

  bool bOnSomePlane = false;

  for (nsUInt32 i = 0; i < PLANE_COUNT; ++i)
  {
    const nsPositionOnPlane::Enum pos = m_Planes[i].GetObjectPosition(box);

    if (pos == nsPositionOnPlane::Back)
      continue;

    if (pos == nsPositionOnPlane::Front)
      return nsVolumePosition::Outside;

    bOnSomePlane = true;
  }

  if (bOnSomePlane)
    return nsVolumePosition::Intersecting;

  return nsVolumePosition::Inside;
}

void nsFrustum::InvertFrustum()
{
  for (nsUInt32 i = 0; i < PLANE_COUNT; ++i)
    m_Planes[i].Flip();
}

void nsFrustum::ComputeCornerPoints(nsVec3 out_pPoints[FrustumCorner::CORNER_COUNT]) const
{
  // clang-format off
  nsPlane::GetPlanesIntersectionPoint(m_Planes[NearPlane], m_Planes[TopPlane], m_Planes[LeftPlane], out_pPoints[FrustumCorner::NearTopLeft]).IgnoreResult();
  nsPlane::GetPlanesIntersectionPoint(m_Planes[NearPlane], m_Planes[TopPlane], m_Planes[RightPlane], out_pPoints[FrustumCorner::NearTopRight]).IgnoreResult();
  nsPlane::GetPlanesIntersectionPoint(m_Planes[NearPlane], m_Planes[BottomPlane], m_Planes[LeftPlane], out_pPoints[FrustumCorner::NearBottomLeft]).IgnoreResult();
  nsPlane::GetPlanesIntersectionPoint(m_Planes[NearPlane], m_Planes[BottomPlane], m_Planes[RightPlane], out_pPoints[FrustumCorner::NearBottomRight]).IgnoreResult();

  nsPlane::GetPlanesIntersectionPoint(m_Planes[FarPlane], m_Planes[TopPlane], m_Planes[LeftPlane], out_pPoints[FrustumCorner::FarTopLeft]).IgnoreResult();
  nsPlane::GetPlanesIntersectionPoint(m_Planes[FarPlane], m_Planes[TopPlane], m_Planes[RightPlane], out_pPoints[FrustumCorner::FarTopRight]).IgnoreResult();
  nsPlane::GetPlanesIntersectionPoint(m_Planes[FarPlane], m_Planes[BottomPlane], m_Planes[LeftPlane], out_pPoints[FrustumCorner::FarBottomLeft]).IgnoreResult();
  nsPlane::GetPlanesIntersectionPoint(m_Planes[FarPlane], m_Planes[BottomPlane], m_Planes[RightPlane], out_pPoints[FrustumCorner::FarBottomRight]).IgnoreResult();
  // clang-format on
}

nsFrustum nsFrustum::MakeFromMVP(const nsMat4& mModelViewProjection0, nsClipSpaceDepthRange::Enum depthRange, nsHandedness::Enum handedness)
{
  nsMat4 ModelViewProjection = mModelViewProjection0;
  nsGraphicsUtils::ConvertProjectionMatrixDepthRange(ModelViewProjection, depthRange, nsClipSpaceDepthRange::MinusOneToOne);

  nsVec4 planes[6];

  if (handedness == nsHandedness::LeftHanded)
  {
    ModelViewProjection.SetRow(0, -ModelViewProjection.GetRow(0));
  }

  planes[LeftPlane] = -ModelViewProjection.GetRow(3) - ModelViewProjection.GetRow(0);
  planes[RightPlane] = -ModelViewProjection.GetRow(3) + ModelViewProjection.GetRow(0);
  planes[BottomPlane] = -ModelViewProjection.GetRow(3) - ModelViewProjection.GetRow(1);
  planes[TopPlane] = -ModelViewProjection.GetRow(3) + ModelViewProjection.GetRow(1);
  planes[NearPlane] = -ModelViewProjection.GetRow(3) - ModelViewProjection.GetRow(2);
  planes[FarPlane] = -ModelViewProjection.GetRow(3) + ModelViewProjection.GetRow(2);

  // Normalize planes
  for (int p = 0; p < 6; ++p)
  {
    const float len = planes[p].GetAsVec3().GetLength();
    // doing the division here manually since we want to accept the case where length is 0 (infinite plane)
    const float invLen = 1.f / len;
    planes[p].x *= nsMath::IsFinite(invLen) ? invLen : 0.f;
    planes[p].y *= nsMath::IsFinite(invLen) ? invLen : 0.f;
    planes[p].z *= nsMath::IsFinite(invLen) ? invLen : 0.f;
    planes[p].w *= invLen;
  }

  // The last matrix row is giving the camera's plane, which means its normal is
  // also the camera's viewing direction.
  const nsVec3 cameraViewDirection = ModelViewProjection.GetRow(3).GetAsVec3();

  // Making sure the near/far plane is always closest/farthest. The way we derive the
  // planes always yields the closer plane pointing towards the camera and the farther
  // plane pointing away from the camera, so flip when that relationship inverts.
  if (planes[FarPlane].GetAsVec3().Dot(cameraViewDirection) < 0)
  {
    NS_ASSERT_DEBUG(planes[NearPlane].GetAsVec3().Dot(cameraViewDirection) >= 0, "");
    nsMath::Swap(planes[NearPlane], planes[FarPlane]);
  }

  // In case we have an infinity far plane projection, the normal is invalid.
  // We'll just take the mirrored normal from the near plane.
  NS_ASSERT_DEBUG(planes[NearPlane].IsValid(), "Near plane is expected to be non-nan and finite at this point!");
  if (nsMath::Abs(planes[FarPlane].w) == nsMath::Infinity<float>())
  {
    planes[FarPlane] = (-planes[NearPlane].GetAsVec3()).GetAsVec4(planes[FarPlane].w);
  }

  static_assert(offsetof(nsPlane, m_vNormal) == offsetof(nsVec4, x) && offsetof(nsPlane, m_fNegDistance) == offsetof(nsVec4, w));

  nsFrustum res;
  nsMemoryUtils::Copy(res.m_Planes, (nsPlane*)planes, 6);
  return res;
}

nsFrustum nsFrustum::MakeFromFOV(const nsVec3& vPosition, const nsVec3& vForwards, const nsVec3& vUp, nsAngle fovX, nsAngle fovY, float fNearPlane, float fFarPlane)
{
  NS_ASSERT_DEBUG(nsMath::Abs(vForwards.GetNormalized().Dot(vUp.GetNormalized())) < 0.999f, "Up dir must be different from forward direction");

  const nsVec3 vForwardsNorm = vForwards.GetNormalized();
  const nsVec3 vRightNorm = vForwards.CrossRH(vUp).GetNormalized();
  const nsVec3 vUpNorm = vRightNorm.CrossRH(vForwards).GetNormalized();

  nsFrustum res;

  // Near Plane
  res.m_Planes[NearPlane] = nsPlane::MakeFromNormalAndPoint(-vForwardsNorm, vPosition + fNearPlane * vForwardsNorm);

  // Far Plane
  res.m_Planes[FarPlane] = nsPlane::MakeFromNormalAndPoint(vForwardsNorm, vPosition + fFarPlane * vForwardsNorm);

  // Making sure the near/far plane is always closest/farthest.
  if (fNearPlane > fFarPlane)
  {
    nsMath::Swap(res.m_Planes[NearPlane], res.m_Planes[FarPlane]);
  }

  nsMat3 mLocalFrame;
  mLocalFrame.SetColumn(0, vRightNorm);
  mLocalFrame.SetColumn(1, vUpNorm);
  mLocalFrame.SetColumn(2, -vForwardsNorm);

  const float fCosFovX = nsMath::Cos(fovX * 0.5f);
  const float fSinFovX = nsMath::Sin(fovX * 0.5f);

  const float fCosFovY = nsMath::Cos(fovY * 0.5f);
  const float fSinFovY = nsMath::Sin(fovY * 0.5f);

  // Left Plane
  {
    nsVec3 vPlaneNormal = mLocalFrame * nsVec3(-fCosFovX, 0, fSinFovX);
    vPlaneNormal.Normalize();

    res.m_Planes[LeftPlane] = nsPlane::MakeFromNormalAndPoint(vPlaneNormal, vPosition);
  }

  // Right Plane
  {
    nsVec3 vPlaneNormal = mLocalFrame * nsVec3(fCosFovX, 0, fSinFovX);
    vPlaneNormal.Normalize();

    res.m_Planes[RightPlane] = nsPlane::MakeFromNormalAndPoint(vPlaneNormal, vPosition);
  }

  // Bottom Plane
  {
    nsVec3 vPlaneNormal = mLocalFrame * nsVec3(0, -fCosFovY, fSinFovY);
    vPlaneNormal.Normalize();

    res.m_Planes[BottomPlane] = nsPlane::MakeFromNormalAndPoint(vPlaneNormal, vPosition);
  }

  // Top Plane
  {
    nsVec3 vPlaneNormal = mLocalFrame * nsVec3(0, fCosFovY, fSinFovY);
    vPlaneNormal.Normalize();

    res.m_Planes[TopPlane] = nsPlane::MakeFromNormalAndPoint(vPlaneNormal, vPosition);
  }

  return res;
}

NS_STATICLINK_FILE(Foundation, Foundation_Math_Implementation_Frustum);
