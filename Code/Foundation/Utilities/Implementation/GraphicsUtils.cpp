/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Utilities/GraphicsUtils.h>

nsResult nsGraphicsUtils::ConvertWorldPosToScreenPos(const nsMat4& mModelViewProjection, const nsUInt32 uiViewportX, const nsUInt32 uiViewportY, const nsUInt32 uiViewportWidth, const nsUInt32 uiViewportHeight, const nsVec3& vPoint, nsVec3& out_vScreenPos, nsClipSpaceDepthRange::Enum depthRange)
{
  const nsVec4 vToProject = vPoint.GetAsVec4(1.0f);

  nsVec4 vClipSpace = mModelViewProjection * vToProject;

  if (vClipSpace.w == 0.0f)
    return NS_FAILURE;

  nsVec3 vProjected = vClipSpace.GetAsVec3() / vClipSpace.w;
  if (vClipSpace.w < 0.0f)
    vProjected.z = -vProjected.z;

  out_vScreenPos.x = uiViewportX + uiViewportWidth * ((vProjected.x * 0.5f) + 0.5f);
  out_vScreenPos.y = uiViewportY + uiViewportHeight * ((vProjected.y * 0.5f) + 0.5f);

  // normalize the output z value to always be in [0; 1] range
  // That means when the projection matrix spits out values between -1 and +1, rescale those values
  if (depthRange == nsClipSpaceDepthRange::MinusOneToOne)
    out_vScreenPos.z = vProjected.z * 0.5f + 0.5f;
  else
    out_vScreenPos.z = vProjected.z;

  return NS_SUCCESS;
}

nsResult nsGraphicsUtils::ConvertScreenPosToWorldPos(
  const nsMat4& mInverseModelViewProjection, const nsUInt32 uiViewportX, const nsUInt32 uiViewportY, const nsUInt32 uiViewportWidth, const nsUInt32 uiViewportHeight, const nsVec3& vScreenPos, nsVec3& out_vPoint, nsVec3* out_pDirection, nsClipSpaceDepthRange::Enum depthRange)
{
  nsVec3 vClipSpace = vScreenPos;

  // From window coordinates to [0; 1] range
  vClipSpace.x = (vClipSpace.x - uiViewportX) / uiViewportWidth;
  vClipSpace.y = (vClipSpace.y - uiViewportY) / uiViewportHeight;

  // Map to range [-1; 1]
  vClipSpace.x = vClipSpace.x * 2.0f - 1.0f;
  vClipSpace.y = vClipSpace.y * 2.0f - 1.0f;

  // The OpenGL matrix expects the z values to be between -1 and +1, so rescale the incoming value to that range
  if (depthRange == nsClipSpaceDepthRange::MinusOneToOne)
    vClipSpace.z = vClipSpace.z * 2.0f - 1.0f;

  nsVec4 vToUnProject = vClipSpace.GetAsVec4(1.0f);

  nsVec4 vWorldSpacePoint = mInverseModelViewProjection * vToUnProject;

  if (vWorldSpacePoint.w == 0.0f)
    return NS_FAILURE;

  out_vPoint = vWorldSpacePoint.GetAsVec3() / vWorldSpacePoint.w;

  if (out_pDirection != nullptr)
  {
    vToUnProject.z += 0.1f; // a point that is a bit further away

    const nsVec4 vWorldSpacePoint2 = mInverseModelViewProjection * vToUnProject;

    NS_ASSERT_DEV(vWorldSpacePoint2.w != 0.0f, "It should not be possible that the first projected point has a w other than zero, but the second one has!");

    const nsVec3 vPoint2 = vWorldSpacePoint2.GetAsVec3() / vWorldSpacePoint2.w;

    *out_pDirection = (vPoint2 - out_vPoint).GetNormalized();
  }

  return NS_SUCCESS;
}

nsResult nsGraphicsUtils::ConvertScreenPosToWorldPos(const nsMat4d& mInverseModelViewProjection, const nsUInt32 uiViewportX, const nsUInt32 uiViewportY, const nsUInt32 uiViewportWidth, const nsUInt32 uiViewportHeight, const nsVec3& vScreenPos, nsVec3& out_vPoint, nsVec3* out_pDirection /*= nullptr*/,
  nsClipSpaceDepthRange::Enum depthRange /*= nsClipSpaceDepthRange::Default*/)
{
  nsVec3 vClipSpace = vScreenPos;

  // From window coordinates to [0; 1] range
  vClipSpace.x = (vClipSpace.x - uiViewportX) / uiViewportWidth;
  vClipSpace.y = (vClipSpace.y - uiViewportY) / uiViewportHeight;

  // Map to range [-1; 1]
  vClipSpace.x = vClipSpace.x * 2.0f - 1.0f;
  vClipSpace.y = vClipSpace.y * 2.0f - 1.0f;

  // The OpenGL matrix expects the z values to be between -1 and +1, so rescale the incoming value to that range
  if (depthRange == nsClipSpaceDepthRange::MinusOneToOne)
    vClipSpace.z = vClipSpace.z * 2.0f - 1.0f;

  nsVec4d vToUnProject = nsVec4d(vClipSpace.x, vClipSpace.y, vClipSpace.z, 1.0);

  nsVec4d vWorldSpacePoint = mInverseModelViewProjection * vToUnProject;

  if (vWorldSpacePoint.w == 0.0)
    return NS_FAILURE;

  nsVec3d outTemp = vWorldSpacePoint.GetAsVec3() / vWorldSpacePoint.w;
  out_vPoint.Set((float)outTemp.x, (float)outTemp.y, (float)outTemp.z);

  if (out_pDirection != nullptr)
  {
    vToUnProject.z += 0.1f; // a point that is a bit further away

    const nsVec4d vWorldSpacePoint2 = mInverseModelViewProjection * vToUnProject;

    NS_ASSERT_DEV(vWorldSpacePoint2.w != 0.0, "It should not be possible that the first projected point has a w other than zero, but the second one has!");

    const nsVec3d vPoint2 = vWorldSpacePoint2.GetAsVec3() / vWorldSpacePoint2.w;

    nsVec3d outDir = (vPoint2 - outTemp).GetNormalized();
    out_pDirection->Set((float)outDir.x, (float)outDir.y, (float)outDir.z);
  }

  return NS_SUCCESS;
}

bool nsGraphicsUtils::IsTriangleFlipRequired(const nsMat3& mTransformation)
{
  return (mTransformation.GetColumn(0).CrossRH(mTransformation.GetColumn(1)).Dot(mTransformation.GetColumn(2)) < 0.0f);
}

void nsGraphicsUtils::ConvertProjectionMatrixDepthRange(nsMat4& inout_mMatrix, nsClipSpaceDepthRange::Enum srcDepthRange, nsClipSpaceDepthRange::Enum dstDepthRange)
{
  // exclude identity transformations
  if (srcDepthRange == dstDepthRange)
    return;

  nsVec4 row2 = inout_mMatrix.GetRow(2);
  nsVec4 row3 = inout_mMatrix.GetRow(3);

  // only need to check SrcDepthRange, the rest is the logical conclusion from being not equal
  if (srcDepthRange == nsClipSpaceDepthRange::MinusOneToOne /*&& DstDepthRange == nsClipSpaceDepthRange::ZeroToOne*/)
  {
    // map z => (z + w)/2
    row2 += row3;
    row2 *= 0.5f;
  }
  else // if (SrcDepthRange == nsClipSpaceDepthRange::ZeroToOne && DstDepthRange == nsClipSpaceDepthRange::MinusOneToOne)
  {
    // map z => 2z - w
    row2 += row2;
    row2 -= row3;
  }


  inout_mMatrix.SetRow(2, row2);
  inout_mMatrix.SetRow(3, row3);
}

void nsGraphicsUtils::ExtractPerspectiveMatrixFieldOfView(const nsMat4& mProjectionMatrix, nsAngle& out_fovX, nsAngle& out_fovY)
{

  const nsVec3 row0 = mProjectionMatrix.GetRow(0).GetAsVec3();
  const nsVec3 row1 = mProjectionMatrix.GetRow(1).GetAsVec3();
  const nsVec3 row3 = mProjectionMatrix.GetRow(3).GetAsVec3();

  const nsVec3 leftPlane = (row3 + row0).GetNormalized();
  const nsVec3 rightPlane = (row3 - row0).GetNormalized();
  const nsVec3 bottomPlane = (row3 + row1).GetNormalized();
  const nsVec3 topPlane = (row3 - row1).GetNormalized();

  out_fovX = nsAngle::MakeFromRadian(nsMath::Pi<float>()) - nsMath::ACos(leftPlane.Dot(rightPlane));
  out_fovY = nsAngle::MakeFromRadian(nsMath::Pi<float>()) - nsMath::ACos(topPlane.Dot(bottomPlane));
}

void nsGraphicsUtils::ExtractPerspectiveMatrixFieldOfView(const nsMat4& mProjectionMatrix, nsAngle& out_fovLeft, nsAngle& out_fovRight, nsAngle& out_fovBottom, nsAngle& out_fovTop, nsClipSpaceYMode::Enum range)
{
  const nsVec3 row0 = mProjectionMatrix.GetRow(0).GetAsVec3();
  const nsVec3 row1 = mProjectionMatrix.GetRow(1).GetAsVec3();
  const nsVec3 row3 = mProjectionMatrix.GetRow(3).GetAsVec3();

  const nsVec3 leftPlane = (row3 + row0).GetNormalized();
  const nsVec3 rightPlane = (row3 - row0).GetNormalized();
  const nsVec3 bottomPlane = (row3 + row1).GetNormalized();
  const nsVec3 topPlane = (row3 - row1).GetNormalized();

  out_fovLeft = -nsMath::ACos(leftPlane.Dot(nsVec3(1.0f, 0, 0)));
  out_fovRight = nsAngle::MakeFromRadian(nsMath::Pi<float>()) - nsMath::ACos(rightPlane.Dot(nsVec3(1.0f, 0, 0)));
  out_fovBottom = -nsMath::ACos(bottomPlane.Dot(nsVec3(0, 1.0f, 0)));
  out_fovTop = nsAngle::MakeFromRadian(nsMath::Pi<float>()) - nsMath::ACos(topPlane.Dot(nsVec3(0, 1.0f, 0)));

  if (range == nsClipSpaceYMode::Flipped)
    nsMath::Swap(out_fovBottom, out_fovTop);
}

nsResult nsGraphicsUtils::ExtractPerspectiveMatrixFieldOfView(const nsMat4& mProjectionMatrix, float& out_fLeft, float& out_fRight, float& out_fBottom, float& out_fTop, nsClipSpaceDepthRange::Enum depthRange, nsClipSpaceYMode::Enum range)
{
  float fNear, fFar;
  NS_SUCCEED_OR_RETURN(ExtractNearAndFarClipPlaneDistances(fNear, fFar, mProjectionMatrix, depthRange));
  // Compensate for inverse-Z.
  const float fMinDepth = nsMath::Min(fNear, fFar);

  nsAngle fFovLeft;
  nsAngle fFovRight;
  nsAngle fFovBottom;
  nsAngle fFovTop;
  ExtractPerspectiveMatrixFieldOfView(mProjectionMatrix, fFovLeft, fFovRight, fFovBottom, fFovTop, range);

  out_fLeft = nsMath::Tan(fFovLeft) * fMinDepth;
  out_fRight = nsMath::Tan(fFovRight) * fMinDepth;
  out_fBottom = nsMath::Tan(fFovBottom) * fMinDepth;
  out_fTop = nsMath::Tan(fFovTop) * fMinDepth;
  return NS_SUCCESS;
}

nsResult nsGraphicsUtils::ExtractNearAndFarClipPlaneDistances(float& out_fNear, float& out_fFar, const nsMat4& mProjectionMatrix, nsClipSpaceDepthRange::Enum depthRange)
{
  const nsVec4 row2 = mProjectionMatrix.GetRow(2);
  const nsVec4 row3 = mProjectionMatrix.GetRow(3);

  nsVec4 nearPlane = row2;

  if (depthRange == nsClipSpaceDepthRange::MinusOneToOne)
  {
    nearPlane += row3;
  }

  const nsVec4 farPlane = row3 - row2;

  const float nearLength = nearPlane.GetAsVec3().GetLength();
  const float farLength = farPlane.GetAsVec3().GetLength();

  const float nearW = nsMath::Abs(nearPlane.w);
  const float farW = nsMath::Abs(farPlane.w);

  if ((nearLength < nsMath::SmallEpsilon<float>() && farLength < nsMath::SmallEpsilon<float>()) ||
      nearW < nsMath::SmallEpsilon<float>() || farW < nsMath::SmallEpsilon<float>())
  {
    return NS_FAILURE;
  }

  const float fNear = nearW / nearLength;
  const float fFar = farW / farLength;

  if (nsMath::IsEqual(fNear, fFar, nsMath::SmallEpsilon<float>()))
  {
    return NS_FAILURE;
  }

  out_fNear = fNear;
  out_fFar = fFar;

  return NS_SUCCESS;
}

nsPlane nsGraphicsUtils::ComputeInterpolatedFrustumPlane(FrustumPlaneInterpolation direction, float fLerpFactor, const nsMat4& mProjectionMatrix, nsClipSpaceDepthRange::Enum depthRange)
{
  nsVec4 rowA;
  nsVec4 rowB = mProjectionMatrix.GetRow(3);
  const float factorMinus1to1 = (fLerpFactor - 0.5f) * 2.0f; // bring into [-1; +1] range

  switch (direction)
  {
    case FrustumPlaneInterpolation::LeftToRight:
    {
      rowA = mProjectionMatrix.GetRow(0);
      rowB *= factorMinus1to1;
      break;
    }

    case FrustumPlaneInterpolation::BottomToTop:
    {
      rowA = mProjectionMatrix.GetRow(1);
      rowB *= factorMinus1to1;
      break;
    }

    case FrustumPlaneInterpolation::NearToFar:
      rowA = mProjectionMatrix.GetRow(2);

      if (depthRange == nsClipSpaceDepthRange::ZeroToOne)
        rowB *= fLerpFactor; // [0; 1] range
      else
        rowB *= factorMinus1to1;
      break;
  }

  nsPlane res;
  res.m_vNormal = rowA.GetAsVec3() - rowB.GetAsVec3();
  res.m_fNegDistance = (rowA.w - rowB.w) / res.m_vNormal.GetLengthAndNormalize();

  return res;
}

nsMat4 nsGraphicsUtils::CreatePerspectiveProjectionMatrix(float fViewWidth, float fViewHeight, float fNearZ, float fFarZ, nsClipSpaceDepthRange::Enum depthRange, nsClipSpaceYMode::Enum range, nsHandedness::Enum handedness)
{
  const float vw = fViewWidth * 0.5f;
  const float vh = fViewHeight * 0.5f;

  return CreatePerspectiveProjectionMatrix(-vw, vw, -vh, vh, fNearZ, fFarZ, depthRange, range, handedness);
}

nsMat4 nsGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovX(nsAngle fieldOfViewX, float fAspectRatioWidthDivHeight, float fNearZ, float fFarZ, nsClipSpaceDepthRange::Enum depthRange, nsClipSpaceYMode::Enum range, nsHandedness::Enum handedness)
{
  // Taking the minimum allows the function to be used to create
  // inverse z matrices (fNearZ > fFarZ) as well.
  const float xm = nsMath::Min(fNearZ, fFarZ) * nsMath::Tan(fieldOfViewX * 0.5f);
  const float ym = xm / fAspectRatioWidthDivHeight;

  return CreatePerspectiveProjectionMatrix(-xm, xm, -ym, ym, fNearZ, fFarZ, depthRange, range, handedness);
}

nsMat4 nsGraphicsUtils::CreatePerspectiveProjectionMatrixFromFovY(nsAngle fieldOfViewY, float fAspectRatioWidthDivHeight, float fNearZ, float fFarZ, nsClipSpaceDepthRange::Enum depthRange, nsClipSpaceYMode::Enum range, nsHandedness::Enum handedness)
{
  // Taking the minimum allows the function to be used to create
  // inverse z matrices (fNearZ > fFarZ) as well.
  const float ym = nsMath::Min(fNearZ, fFarZ) * nsMath::Tan(fieldOfViewY * 0.5);
  const float xm = ym * fAspectRatioWidthDivHeight;

  return CreatePerspectiveProjectionMatrix(-xm, xm, -ym, ym, fNearZ, fFarZ, depthRange, range, handedness);
}

nsMat4 nsGraphicsUtils::CreateOrthographicProjectionMatrix(float fViewWidth, float fViewHeight, float fNearZ, float fFarZ, nsClipSpaceDepthRange::Enum depthRange, nsClipSpaceYMode::Enum range, nsHandedness::Enum handedness)
{
  return CreateOrthographicProjectionMatrix(-fViewWidth * 0.5f, fViewWidth * 0.5f, -fViewHeight * 0.5f, fViewHeight * 0.5f, fNearZ, fFarZ, depthRange, range, handedness);
}

nsMat4 nsGraphicsUtils::CreateOrthographicProjectionMatrix(float fLeft, float fRight, float fBottom, float fTop, float fNearZ, float fFarZ, nsClipSpaceDepthRange::Enum depthRange, nsClipSpaceYMode::Enum range, nsHandedness::Enum handedness)
{
  NS_ASSERT_DEBUG(nsMath::IsFinite(fNearZ) && nsMath::IsFinite(fFarZ), "Infinite plane values are not supported for orthographic projections!");

  nsMat4 res;
  res.SetIdentity();

  if (range == nsClipSpaceYMode::Flipped)
  {
    nsMath::Swap(fBottom, fTop);
  }

  const float fOneDivFarMinusNear = 1.0f / (fFarZ - fNearZ);
  const float fOneDivRightMinusLeft = 1.0f / (fRight - fLeft);
  const float fOneDivTopMinusBottom = 1.0f / (fTop - fBottom);

  res.Element(0, 0) = 2.0f / (fRight - fLeft);

  res.Element(1, 1) = 2.0f / (fTop - fBottom);

  res.Element(3, 0) = -(fLeft + fRight) * fOneDivRightMinusLeft;
  res.Element(3, 1) = -(fTop + fBottom) * fOneDivTopMinusBottom;


  if (depthRange == nsClipSpaceDepthRange::MinusOneToOne)
  {
    // The OpenGL Way: http://wiki.delphigl.com/index.php/glFrustum
    res.Element(2, 2) = -2.0f * fOneDivFarMinusNear;
    res.Element(3, 2) = -(fFarZ + fNearZ) * fOneDivFarMinusNear;
  }
  else
  {
    // The Left-Handed Direct3D Way: https://docs.microsoft.com/windows/win32/direct3d9/d3dxmatrixorthooffcenterlh
    // The Right-Handed Direct3D Way: https://docs.microsoft.com/windows/win32/direct3d9/d3dxmatrixorthooffcenterrh

    res.Element(2, 2) = -1.0f * fOneDivFarMinusNear;
    res.Element(3, 2) = -fNearZ * fOneDivFarMinusNear;
  }

  if (handedness == nsHandedness::LeftHanded)
  {
    res.SetColumn(2, -res.GetColumn(2));
  }

  return res;
}

nsMat4 nsGraphicsUtils::CreatePerspectiveProjectionMatrix(float fLeft, float fRight, float fBottom, float fTop, float fNearZ, float fFarZ, nsClipSpaceDepthRange::Enum depthRange, nsClipSpaceYMode::Enum range, nsHandedness::Enum handedness)
{
  NS_ASSERT_DEBUG(nsMath::IsFinite(fNearZ) || nsMath::IsFinite(fFarZ), "fNearZ and fFarZ cannot both be infinite at the same time!");

  nsMat4 res;
  res.SetZero();

  if (range == nsClipSpaceYMode::Flipped)
  {
    nsMath::Swap(fBottom, fTop);
  }

  // Taking the minimum of the two plane values allows
  // this function to also be used to create inverse-z
  // matrices by specifying values of fNearZ > fFarZ.
  // Otherwise the x and y scaling values will be wrong
  // in the final matrix.
  const float fMinPlane = nsMath::Min(fNearZ, fFarZ);
  const float fTwoNearZ = fMinPlane + fMinPlane;
  const float fOneDivRightMinusLeft = 1.0f / (fRight - fLeft);
  const float fOneDivTopMinusBottom = 1.0f / (fTop - fBottom);

  res.Element(0, 0) = fTwoNearZ * fOneDivRightMinusLeft;

  res.Element(1, 1) = fTwoNearZ * fOneDivTopMinusBottom;

  res.Element(2, 0) = (fLeft + fRight) * fOneDivRightMinusLeft;
  res.Element(2, 1) = (fTop + fBottom) * fOneDivTopMinusBottom;
  res.Element(2, 3) = -1.0f;

  // If either fNearZ or fFarZ is infinite, one can derive the resulting z-transformation by using limit math
  // and letting the respective variable approach infinity in the original expressions for P(2, 2) and P(3, 2).
  // The result is that a couple of terms from the original fraction get reduced to 0 by being divided by infinity,
  // which fortunately yields 1) finite and 2) much simpler expressions for P(2, 2) and P(3, 2).
  if (depthRange == nsClipSpaceDepthRange::MinusOneToOne)
  {
    // The OpenGL Way: http://wiki.delphigl.com/index.php/glFrustum
    // Algebraically reordering the z-row fractions from the above source in a way so infinite fNearZ or fFarZ will zero out
    // instead of producing NaNs due to inf/inf divisions will yield these generalized formulas which could be used instead
    // of the branching below. Insert infinity for either fNearZ or fFarZ to see that these will yield exactly these simplifications:
    //res.Element(2, 2) = 1.f / (fNearZ / fFarZ - 1.f) + 1.f / (1.f - fFarZ / fNearZ);
    //res.Element(3, 2) = 2.f / (1.f / fFarZ - 1.f / fNearZ);
    if (!nsMath::IsFinite(fNearZ))
    {
      res.Element(2, 2) = 1.f;
      res.Element(3, 2) = 2.f * fFarZ;
    }
    else if (!nsMath::IsFinite(fFarZ))
    {
      res.Element(2, 2) = -1.f;
      res.Element(3, 2) = -2.f * fNearZ;
    }
    else
    {
      const float fOneDivNearMinusFar = 1.0f / (fNearZ - fFarZ);
      res.Element(2, 2) = (fFarZ + fNearZ) * fOneDivNearMinusFar;
      res.Element(3, 2) = 2 * fFarZ * fNearZ * fOneDivNearMinusFar;
    }
  }
  else
  {
    // The Left-Handed Direct3D Way: https://docs.microsoft.com/windows/win32/direct3d9/d3dxmatrixperspectiveoffcenterlh
    // The Right-Handed Direct3D Way: https://docs.microsoft.com/windows/win32/direct3d9/d3dxmatrixperspectiveoffcenterrh
    // Algebraically reordering the z-row fractions from the above source in a way so infinite fNearZ or fFarZ will zero out
    // instead of producing NaNs due to inf/inf divisions will yield these generalized formulas which could be used instead
    // of the branching below. Insert infinity for either fNearZ or fFarZ to see that these will yield exactly these simplifications:
    //res.Element(2, 2) = 1.f / (fNearZ / fFarZ - 1.f);
    //res.Element(3, 2) = 1.f / (1.f / fFarZ - 1.f / fNearZ);
    if (!nsMath::IsFinite(fNearZ))
    {
      res.Element(2, 2) = 0.f;
      res.Element(3, 2) = fFarZ;
    }
    else if (!nsMath::IsFinite(fFarZ))
    {
      res.Element(2, 2) = -1.f;
      res.Element(3, 2) = -fNearZ;
    }
    else
    {
      const float fOneDivNearMinusFar = 1.0f / (fNearZ - fFarZ);
      res.Element(2, 2) = fFarZ * fOneDivNearMinusFar;
      res.Element(3, 2) = fFarZ * fNearZ * fOneDivNearMinusFar;
    }
  }

  if (handedness == nsHandedness::LeftHanded)
  {
    res.SetColumn(2, -res.GetColumn(2));
  }

  return res;
}

nsMat3 nsGraphicsUtils::CreateLookAtViewMatrix(const nsVec3& vTarget, const nsVec3& vUpDir, nsHandedness::Enum handedness)
{
  NS_ASSERT_DEBUG(!vTarget.IsZero(), "The target must not be at the origin.");

  nsVec3 vLookDir = vTarget;
  vLookDir.NormalizeIfNotZero(nsVec3::MakeAxisX()).IgnoreResult();

  nsVec3 vNormalizedUpDir = vUpDir.GetNormalized();

  if (nsMath::Abs(vLookDir.Dot(vNormalizedUpDir)) > 0.9999f) // less than 1 degree difference -> problem
  {
    // use some arbitrary other orthogonal vector as UP
    vNormalizedUpDir = vLookDir.GetOrthogonalVector();
  }

  nsMat3 res;

  const nsVec3 zaxis = (handedness == nsHandedness::RightHanded) ? -vLookDir : vLookDir;
  const nsVec3 xaxis = vNormalizedUpDir.CrossRH(zaxis).GetNormalized();
  const nsVec3 yaxis = zaxis.CrossRH(xaxis);

  res.SetRow(0, xaxis);
  res.SetRow(1, yaxis);
  res.SetRow(2, zaxis);

  return res;
}

nsMat3 nsGraphicsUtils::CreateInverseLookAtViewMatrix(const nsVec3& vTarget, const nsVec3& vUpDir, nsHandedness::Enum handedness)
{
  NS_ASSERT_DEBUG(!vTarget.IsZero(), "The target must not be at the origin.");

  nsVec3 vLookDir = vTarget;
  vLookDir.NormalizeIfNotZero(nsVec3::MakeAxisX()).IgnoreResult();

  nsVec3 vNormalizedUpDir = vUpDir.GetNormalized();

  if (nsMath::Abs(vLookDir.Dot(vNormalizedUpDir)) > 0.9999f) // less than 1 degree difference -> problem
  {
    // use some arbitrary other orthogonal vector as UP
    vNormalizedUpDir = vLookDir.GetOrthogonalVector();
  }

  nsMat3 res;

  const nsVec3 zaxis = (handedness == nsHandedness::RightHanded) ? -vLookDir : vLookDir;
  const nsVec3 xaxis = vNormalizedUpDir.CrossRH(zaxis).GetNormalized();
  const nsVec3 yaxis = zaxis.CrossRH(xaxis);

  res.SetColumn(0, xaxis);
  res.SetColumn(1, yaxis);
  res.SetColumn(2, zaxis);

  return res;
}

nsMat4 nsGraphicsUtils::CreateLookAtViewMatrix(const nsVec3& vEyePos, const nsVec3& vLookAtPos, const nsVec3& vUpDir, nsHandedness::Enum handedness)
{
  const nsMat3 rotation = CreateLookAtViewMatrix(vLookAtPos - vEyePos, vUpDir, handedness);

  nsMat4 res;
  res.SetRotationalPart(rotation);
  res.SetTranslationVector(rotation * -vEyePos);
  res.SetRow(3, nsVec4(0, 0, 0, 1));
  return res;
}

nsMat4 nsGraphicsUtils::CreateInverseLookAtViewMatrix(const nsVec3& vEyePos, const nsVec3& vLookAtPos, const nsVec3& vUpDir, nsHandedness::Enum handedness)
{
  const nsMat3 rotation = CreateInverseLookAtViewMatrix(vLookAtPos - vEyePos, vUpDir, handedness);

  nsMat4 res;
  res.SetRotationalPart(rotation);
  res.SetTranslationVector(vEyePos);
  res.SetRow(3, nsVec4(0, 0, 0, 1));
  return res;
}

nsMat4 nsGraphicsUtils::CreateViewMatrix(const nsVec3& vPosition, const nsVec3& vForwardDir, const nsVec3& vRightDir, const nsVec3& vUpDir, nsHandedness::Enum handedness)
{
  nsMat4 res;
  res.SetIdentity();

  nsVec3 xaxis, yaxis, zaxis;

  if (handedness == nsHandedness::LeftHanded)
  {
    xaxis = vRightDir;
    yaxis = vUpDir;
    zaxis = vForwardDir;
  }
  else
  {
    xaxis = vRightDir;
    yaxis = vUpDir;
    zaxis = -vForwardDir;
  }

  res.SetRow(0, xaxis.GetAsVec4(0));
  res.SetRow(1, yaxis.GetAsVec4(0));
  res.SetRow(2, zaxis.GetAsVec4(0));
  res.SetTranslationVector(nsVec3(-xaxis.Dot(vPosition), -yaxis.Dot(vPosition), -zaxis.Dot(vPosition)));

  return res;
}

nsMat4 nsGraphicsUtils::CreateInverseViewMatrix(const nsVec3& vPosition, const nsVec3& vForwardDir, const nsVec3& vRightDir, const nsVec3& vUpDir, nsHandedness::Enum handedness)
{
  nsMat4 res;
  res.SetIdentity();

  nsVec3 xaxis, yaxis, zaxis;

  if (handedness == nsHandedness::LeftHanded)
  {
    xaxis = vRightDir;
    yaxis = vUpDir;
    zaxis = vForwardDir;
  }
  else
  {
    xaxis = vRightDir;
    yaxis = vUpDir;
    zaxis = -vForwardDir;
  }

  res.SetColumn(0, xaxis.GetAsVec4(0));
  res.SetColumn(1, yaxis.GetAsVec4(0));
  res.SetColumn(2, zaxis.GetAsVec4(0));
  res.SetTranslationVector(vPosition);

  return res;
}

void nsGraphicsUtils::DecomposeViewMatrix(nsVec3& ref_vPosition, nsVec3& ref_vForwardDir, nsVec3& ref_vRightDir, nsVec3& ref_vUpDir, const nsMat4& mViewMatrix, nsHandedness::Enum handedness)
{
  const nsMat3 rotation = mViewMatrix.GetRotationalPart();

  if (handedness == nsHandedness::LeftHanded)
  {
    ref_vRightDir = rotation.GetRow(0);
    ref_vUpDir = rotation.GetRow(1);
    ref_vForwardDir = rotation.GetRow(2);
  }
  else
  {
    ref_vRightDir = rotation.GetRow(0);
    ref_vUpDir = rotation.GetRow(1);
    ref_vForwardDir = -rotation.GetRow(2);
  }

  ref_vPosition = rotation.GetTranspose() * -mViewMatrix.GetTranslationVector();
}

nsResult nsGraphicsUtils::ComputeBarycentricCoordinates(nsVec3& out_vCoordinates, const nsVec3& a, const nsVec3& b, const nsVec3& c, const nsVec3& p)
{
  // implementation copied from https://gamedev.stackexchange.com/a/49370

  const nsVec3 v0 = b - a;
  const nsVec3 v1 = c - a;
  const nsVec3 v2 = p - a;

  const float d00 = v0.Dot(v0);
  const float d01 = v0.Dot(v1);
  const float d11 = v1.Dot(v1);
  const float d20 = v2.Dot(v0);
  const float d21 = v2.Dot(v1);
  const float denom = d00 * d11 - d01 * d01;

  if (nsMath::IsZero(denom, nsMath::SmallEpsilon<float>()))
    return NS_FAILURE;

  const float invDenom = 1.0f / denom;

  const float v = (d11 * d20 - d01 * d21) * invDenom;
  const float w = (d00 * d21 - d01 * d20) * invDenom;
  const float u = 1.0f - v - w;

  out_vCoordinates.Set(u, v, w);

  return NS_SUCCESS;
}

nsResult nsGraphicsUtils::ComputeBarycentricCoordinates(nsVec3& out_vCoordinates, const nsVec2& a, const nsVec2& b, const nsVec2& c, const nsVec2& p)
{
  // implementation copied from https://gamedev.stackexchange.com/a/63203

  const nsVec2 v0 = b - a;
  const nsVec2 v1 = c - a;
  const nsVec2 v2 = p - a;

  const float denom = v0.x * v1.y - v1.x * v0.y;

  if (nsMath::IsZero(denom, nsMath::SmallEpsilon<float>()))
    return NS_FAILURE;

  const float invDenom = 1.0f / denom;
  const float v = (v2.x * v1.y - v1.x * v2.y) * invDenom;
  const float w = (v0.x * v2.y - v2.x * v0.y) * invDenom;
  const float u = 1.0f - v - w;

  out_vCoordinates.Set(u, v, w);

  return NS_SUCCESS;
}

NS_STATICLINK_FILE(Foundation, Foundation_Utilities_Implementation_GraphicsUtils);
