/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

nsTag::nsTag()


  = default;

bool nsTag::operator==(const nsTag& rhs) const
{
  return m_sTagString == rhs.m_sTagString;
}

bool nsTag::operator!=(const nsTag& rhs) const
{
  return m_sTagString != rhs.m_sTagString;
}

bool nsTag::operator<(const nsTag& rhs) const
{
  return m_sTagString < rhs.m_sTagString;
}

const nsString& nsTag::GetTagString() const
{
  return m_sTagString.GetString();
}

bool nsTag::IsValid() const
{
  return m_uiBlockIndex != 0xFFFFFFFEu;
}
