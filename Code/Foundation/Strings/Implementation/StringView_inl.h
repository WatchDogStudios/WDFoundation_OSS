/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

NS_ALWAYS_INLINE constexpr nsStringView::nsStringView() = default;

NS_ALWAYS_INLINE nsStringView::nsStringView(char* pStart)
  : m_pStart(pStart)
  , m_pEnd(pStart + nsStringUtils::GetStringElementCount(pStart))
{
}

template <typename T>
constexpr NS_ALWAYS_INLINE nsStringView::nsStringView(T pStart, typename std::enable_if<std::is_same<T, const char*>::value, int>::type*)
  : m_pStart(pStart)
  , m_pEnd(pStart + nsStringUtils::GetStringElementCount(pStart))
{
}

template <typename T>
NS_ALWAYS_INLINE nsStringView::nsStringView(const T&& str, typename std::enable_if<std::is_same<T, const char*>::value == false && std::is_convertible<T, const char*>::value, int>::type*)
{
  m_pStart = str;
  m_pEnd = m_pStart + nsStringUtils::GetStringElementCount(m_pStart);
}

NS_ALWAYS_INLINE nsStringView::nsStringView(const char* pStart, const char* pEnd)
{
  NS_ASSERT_DEV(pStart <= pEnd, "It should start BEFORE it ends.");

  m_pStart = pStart;
  m_pEnd = pEnd;
}

constexpr NS_ALWAYS_INLINE nsStringView::nsStringView(const char* pStart, nsUInt32 uiLength)
  : m_pStart(pStart)
  , m_pEnd(pStart + uiLength)
{
}

template <size_t N>
NS_ALWAYS_INLINE nsStringView::nsStringView(const char (&str)[N])
  : m_pStart(str)
  , m_pEnd(str + N - 1)
{
  static_assert(N > 0, "Not a string literal");
  NS_ASSERT_DEBUG(str[N - 1] == '\0', "Not a string literal. Manually cast to 'const char*' if you are trying to pass a const char fixed size array.");
}

template <size_t N>
NS_ALWAYS_INLINE nsStringView::nsStringView(char (&str)[N])
{
  m_pStart = str;
  m_pEnd = m_pStart + nsStringUtils::GetStringElementCount(str, str + N);
}

inline void nsStringView::operator++()
{
  if (!IsValid())
    return;

  nsUnicodeUtils::MoveToNextUtf8(m_pStart, m_pEnd);
}

inline void nsStringView::operator+=(nsUInt32 d)
{
  nsUnicodeUtils::MoveToNextUtf8(m_pStart, m_pEnd, d);
}
NS_ALWAYS_INLINE bool nsStringView::IsValid() const
{
  return (m_pStart != nullptr) && (m_pStart < m_pEnd);
}

NS_ALWAYS_INLINE void nsStringView::SetStartPosition(const char* szCurPos)
{
  NS_ASSERT_DEV((szCurPos >= m_pStart) && (szCurPos <= m_pEnd), "New start position must still be inside the view's range.");

  m_pStart = szCurPos;
}

NS_ALWAYS_INLINE bool nsStringView::IsEmpty() const
{
  return m_pStart == m_pEnd || nsStringUtils::IsNullOrEmpty(m_pStart);
}

NS_ALWAYS_INLINE bool nsStringView::IsEqual(nsStringView sOther) const
{
  return nsStringUtils::IsEqual(m_pStart, sOther.GetStartPointer(), m_pEnd, sOther.GetEndPointer());
}

NS_ALWAYS_INLINE bool nsStringView::IsEqual_NoCase(nsStringView sOther) const
{
  return nsStringUtils::IsEqual_NoCase(m_pStart, sOther.GetStartPointer(), m_pEnd, sOther.GetEndPointer());
}

NS_ALWAYS_INLINE bool nsStringView::StartsWith(nsStringView sStartsWith) const
{
  return nsStringUtils::StartsWith(m_pStart, sStartsWith.GetStartPointer(), m_pEnd, sStartsWith.GetEndPointer());
}

NS_ALWAYS_INLINE bool nsStringView::StartsWith_NoCase(nsStringView sStartsWith) const
{
  return nsStringUtils::StartsWith_NoCase(m_pStart, sStartsWith.GetStartPointer(), m_pEnd, sStartsWith.GetEndPointer());
}

NS_ALWAYS_INLINE bool nsStringView::EndsWith(nsStringView sEndsWith) const
{
  return nsStringUtils::EndsWith(m_pStart, sEndsWith.GetStartPointer(), m_pEnd, sEndsWith.GetEndPointer());
}

NS_ALWAYS_INLINE bool nsStringView::EndsWith_NoCase(nsStringView sEndsWith) const
{
  return nsStringUtils::EndsWith_NoCase(m_pStart, sEndsWith.GetStartPointer(), m_pEnd, sEndsWith.GetEndPointer());
}

NS_ALWAYS_INLINE void nsStringView::Trim(const char* szTrimChars)
{
  return Trim(szTrimChars, szTrimChars);
}

NS_ALWAYS_INLINE void nsStringView::Trim(const char* szTrimCharsStart, const char* szTrimCharsEnd)
{
  if (IsValid())
  {
    nsStringUtils::Trim(m_pStart, m_pEnd, szTrimCharsStart, szTrimCharsEnd);
  }
}

constexpr NS_ALWAYS_INLINE nsStringView operator"" _nssv(const char* pString, size_t uiLen)
{
  return nsStringView(pString, static_cast<nsUInt32>(uiLen));
}

template <typename Container>
void nsStringView::Split(bool bReturnEmptyStrings, Container& ref_output, const char* szSeparator1, const char* szSeparator2 /*= nullptr*/, const char* szSeparator3 /*= nullptr*/, const char* szSeparator4 /*= nullptr*/, const char* szSeparator5 /*= nullptr*/, const char* szSeparator6 /*= nullptr*/) const
{
  ref_output.Clear();

  if (IsEmpty())
    return;

  const nsUInt32 uiParams = 6;

  const nsStringView seps[uiParams] = {szSeparator1, szSeparator2, szSeparator3, szSeparator4, szSeparator5, szSeparator6};

  const char* szReadPos = GetStartPointer();

  while (true)
  {
    const char* szFoundPos = nsUnicodeUtils::GetMaxStringEnd<char>();
    nsUInt32 uiFoundSeparator = 0;

    for (nsUInt32 i = 0; i < uiParams; ++i)
    {
      const char* szFound = nsStringUtils::FindSubString(szReadPos, seps[i].GetStartPointer(), GetEndPointer(), seps[i].GetEndPointer());

      if ((szFound != nullptr) && (szFound < szFoundPos))
      {
        szFoundPos = szFound;
        uiFoundSeparator = i;
      }
    }

    // nothing found
    if (szFoundPos == nsUnicodeUtils::GetMaxStringEnd<char>())
    {
      const nsUInt32 uiLen = nsStringUtils::GetStringElementCount(szReadPos, GetEndPointer());

      if (bReturnEmptyStrings || (uiLen > 0))
        ref_output.PushBack(nsStringView(szReadPos, szReadPos + uiLen));

      return;
    }

    if (bReturnEmptyStrings || (szFoundPos > szReadPos))
      ref_output.PushBack(nsStringView(szReadPos, szFoundPos));

    szReadPos = szFoundPos + seps[uiFoundSeparator].GetElementCount();
  }
}

NS_ALWAYS_INLINE bool operator==(nsStringView lhs, nsStringView rhs)
{
  return lhs.IsEqual(rhs);
}

NS_ALWAYS_INLINE bool operator!=(nsStringView lhs, nsStringView rhs)
{
  return !lhs.IsEqual(rhs);
}

NS_ALWAYS_INLINE bool operator<(nsStringView lhs, nsStringView rhs)
{
  return lhs.Compare(rhs) < 0;
}

NS_ALWAYS_INLINE bool operator<=(nsStringView lhs, nsStringView rhs)
{
  return lhs.Compare(rhs) <= 0;
}

NS_ALWAYS_INLINE bool operator>(nsStringView lhs, nsStringView rhs)
{
  return lhs.Compare(rhs) > 0;
}

NS_ALWAYS_INLINE bool operator>=(nsStringView lhs, nsStringView rhs)
{
  return lhs.Compare(rhs) >= 0;
}
