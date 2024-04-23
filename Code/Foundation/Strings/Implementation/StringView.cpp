/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Strings/StringBuilder.h>
#include <Foundation/Strings/StringUtils.h>
#include <Foundation/Strings/StringView.h>

nsUInt32 nsStringView::GetCharacter() const
{
  if (!IsValid())
    return 0;

  return nsUnicodeUtils::ConvertUtf8ToUtf32(m_pStart);
}

const char* nsStringView::GetData(nsStringBuilder& ref_sTempStorage) const
{
  ref_sTempStorage = *this;
  return ref_sTempStorage.GetData();
}

bool nsStringView::IsEqualN(nsStringView sOther, nsUInt32 uiCharsToCompare) const
{
  return nsStringUtils::IsEqualN(GetStartPointer(), sOther.GetStartPointer(), uiCharsToCompare, GetEndPointer(), sOther.GetEndPointer());
}

bool nsStringView::IsEqualN_NoCase(nsStringView sOther, nsUInt32 uiCharsToCompare) const
{
  return nsStringUtils::IsEqualN_NoCase(GetStartPointer(), sOther.GetStartPointer(), uiCharsToCompare, GetEndPointer(), sOther.GetEndPointer());
}

nsInt32 nsStringView::Compare(nsStringView sOther) const
{
  return nsStringUtils::Compare(GetStartPointer(), sOther.GetStartPointer(), GetEndPointer(), sOther.GetEndPointer());
}

nsInt32 nsStringView::CompareN(nsStringView sOther, nsUInt32 uiCharsToCompare) const
{
  return nsStringUtils::CompareN(GetStartPointer(), sOther.GetStartPointer(), uiCharsToCompare, GetEndPointer(), sOther.GetEndPointer());
}

nsInt32 nsStringView::Compare_NoCase(nsStringView sOther) const
{
  return nsStringUtils::Compare_NoCase(GetStartPointer(), sOther.GetStartPointer(), GetEndPointer(), sOther.GetEndPointer());
}

nsInt32 nsStringView::CompareN_NoCase(nsStringView sOther, nsUInt32 uiCharsToCompare) const
{
  return nsStringUtils::CompareN_NoCase(GetStartPointer(), sOther.GetStartPointer(), uiCharsToCompare, GetEndPointer(), sOther.GetEndPointer());
}

const char* nsStringView::ComputeCharacterPosition(nsUInt32 uiCharacterIndex) const
{
  const char* pos = GetStartPointer();
  nsUnicodeUtils::MoveToNextUtf8(pos, GetEndPointer(), uiCharacterIndex);
  return pos;
}

const char* nsStringView::FindSubString(nsStringView sStringToFind, const char* szStartSearchAt /*= nullptr*/) const
{
  if (szStartSearchAt == nullptr)
    szStartSearchAt = GetStartPointer();

  NS_ASSERT_DEV((szStartSearchAt >= GetStartPointer()) && (szStartSearchAt <= GetEndPointer()), "The given pointer to start searching at is not inside this strings valid range.");

  return nsStringUtils::FindSubString(szStartSearchAt, sStringToFind.GetStartPointer(), GetEndPointer(), sStringToFind.GetEndPointer());
}

const char* nsStringView::FindSubString_NoCase(nsStringView sStringToFind, const char* szStartSearchAt /*= nullptr*/) const
{
  if (szStartSearchAt == nullptr)
    szStartSearchAt = GetStartPointer();

  NS_ASSERT_DEV((szStartSearchAt >= GetStartPointer()) && (szStartSearchAt <= GetEndPointer()), "The given pointer to start searching at is not inside this strings valid range.");

  return nsStringUtils::FindSubString_NoCase(szStartSearchAt, sStringToFind.GetStartPointer(), GetEndPointer(), sStringToFind.GetEndPointer());
}

const char* nsStringView::FindLastSubString(nsStringView sStringToFind, const char* szStartSearchAt /*= nullptr*/) const
{
  if (szStartSearchAt == nullptr)
    szStartSearchAt = GetEndPointer();

  NS_ASSERT_DEV((szStartSearchAt >= GetStartPointer()) && (szStartSearchAt <= GetEndPointer()), "The given pointer to start searching at is not inside this strings valid range.");

  return nsStringUtils::FindLastSubString(GetStartPointer(), sStringToFind.GetStartPointer(), szStartSearchAt, GetEndPointer(), sStringToFind.GetEndPointer());
}

const char* nsStringView::FindLastSubString_NoCase(nsStringView sStringToFind, const char* szStartSearchAt /*= nullptr*/) const
{
  if (szStartSearchAt == nullptr)
    szStartSearchAt = GetEndPointer();

  NS_ASSERT_DEV((szStartSearchAt >= GetStartPointer()) && (szStartSearchAt <= GetEndPointer()), "The given pointer to start searching at is not inside this strings valid range.");

  return nsStringUtils::FindLastSubString_NoCase(GetStartPointer(), sStringToFind.GetStartPointer(), szStartSearchAt, GetEndPointer(), sStringToFind.GetEndPointer());
}

const char* nsStringView::FindWholeWord(const char* szSearchFor, nsStringUtils::NS_CHARACTER_FILTER isDelimiterCB, const char* szStartSearchAt /*= nullptr*/) const
{
  if (szStartSearchAt == nullptr)
    szStartSearchAt = GetStartPointer();

  NS_ASSERT_DEV((szStartSearchAt >= GetStartPointer()) && (szStartSearchAt <= GetEndPointer()), "The given pointer to start searching at is not inside this strings valid range.");

  return nsStringUtils::FindWholeWord(szStartSearchAt, szSearchFor, isDelimiterCB, GetEndPointer());
}

const char* nsStringView::FindWholeWord_NoCase(const char* szSearchFor, nsStringUtils::NS_CHARACTER_FILTER isDelimiterCB, const char* szStartSearchAt /*= nullptr*/) const
{
  if (szStartSearchAt == nullptr)
    szStartSearchAt = GetStartPointer();

  NS_ASSERT_DEV((szStartSearchAt >= GetStartPointer()) && (szStartSearchAt <= GetEndPointer()), "The given pointer to start searching at is not inside this strings valid range.");

  return nsStringUtils::FindWholeWord_NoCase(szStartSearchAt, szSearchFor, isDelimiterCB, GetEndPointer());
}

void nsStringView::Shrink(nsUInt32 uiShrinkCharsFront, nsUInt32 uiShrinkCharsBack)
{
  while (IsValid() && (uiShrinkCharsFront > 0))
  {
    nsUnicodeUtils::MoveToNextUtf8(m_pStart, m_pEnd, 1);
    --uiShrinkCharsFront;
  }

  while (IsValid() && (uiShrinkCharsBack > 0))
  {
    nsUnicodeUtils::MoveToPriorUtf8(m_pEnd, 1);
    --uiShrinkCharsBack;
  }
}

nsStringView nsStringView::GetShrunk(nsUInt32 uiShrinkCharsFront, nsUInt32 uiShrinkCharsBack) const
{
  nsStringView tmp = *this;
  tmp.Shrink(uiShrinkCharsFront, uiShrinkCharsBack);
  return tmp;
}

void nsStringView::ChopAwayFirstCharacterUtf8()
{
  if (IsValid())
  {
    nsUnicodeUtils::MoveToNextUtf8(m_pStart, m_pEnd, 1);
  }
}

void nsStringView::ChopAwayFirstCharacterAscii()
{
  if (IsValid())
  {
    NS_ASSERT_DEBUG(nsUnicodeUtils::IsASCII(*m_pStart), "ChopAwayFirstCharacterAscii() was called on a non-ASCII character.");

    m_pStart += 1;
  }
}

bool nsStringView::TrimWordStart(nsStringView sWord)
{
  const bool bTrimAll = false;

  bool trimmed = false;

  do
  {
    if (!sWord.IsEmpty() && StartsWith_NoCase(sWord))
    {
      Shrink(nsStringUtils::GetCharacterCount(sWord.GetStartPointer(), sWord.GetEndPointer()), 0);
      trimmed = true;
    }

  } while (bTrimAll);

  return trimmed;
}

bool nsStringView::TrimWordEnd(nsStringView sWord)
{
  const bool bTrimAll = false;

  bool trimmed = false;

  do
  {
    if (!sWord.IsEmpty() && EndsWith_NoCase(sWord))
    {
      Shrink(0, nsStringUtils::GetCharacterCount(sWord.GetStartPointer(), sWord.GetEndPointer()));
      trimmed = true;
    }

  } while (bTrimAll);

  return trimmed;
}

nsStringView::iterator nsStringView::GetIteratorFront() const
{
  return begin(*this);
}

nsStringView::reverse_iterator nsStringView::GetIteratorBack() const
{
  return rbegin(*this);
}

bool nsStringView::HasAnyExtension() const
{
  return nsPathUtils::HasAnyExtension(*this);
}

bool nsStringView::HasExtension(nsStringView sExtension) const
{
  return nsPathUtils::HasExtension(*this, sExtension);
}

nsStringView nsStringView::GetFileExtension() const
{
  return nsPathUtils::GetFileExtension(*this);
}

nsStringView nsStringView::GetFileName() const
{
  return nsPathUtils::GetFileName(*this);
}

nsStringView nsStringView::GetFileNameAndExtension() const
{
  return nsPathUtils::GetFileNameAndExtension(*this);
}

nsStringView nsStringView::GetFileDirectory() const
{
  return nsPathUtils::GetFileDirectory(*this);
}

bool nsStringView::IsAbsolutePath() const
{
  return nsPathUtils::IsAbsolutePath(*this);
}

bool nsStringView::IsRelativePath() const
{
  return nsPathUtils::IsRelativePath(*this);
}

bool nsStringView::IsRootedPath() const
{
  return nsPathUtils::IsRootedPath(*this);
}

nsStringView nsStringView::GetRootedPathRootName() const
{
  return nsPathUtils::GetRootedPathRootName(*this);
}

NS_STATICLINK_FILE(Foundation, Foundation_Strings_Implementation_StringView);
