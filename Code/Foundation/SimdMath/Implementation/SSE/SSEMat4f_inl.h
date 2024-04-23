/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

NS_ALWAYS_INLINE void nsSimdMat4f::Transpose()
{
  _MM_TRANSPOSE4_PS(m_col0.m_v, m_col1.m_v, m_col2.m_v, m_col3.m_v);
}
