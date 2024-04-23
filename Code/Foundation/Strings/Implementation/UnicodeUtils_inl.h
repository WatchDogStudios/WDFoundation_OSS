/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

/*
You can classify bytes in a UTF-8 stream as follows:
  With the high bit set to 0, it's a single byte value.
  With the two high bits set to 10, it's a continuation byte (the second, third or fourth byte in a UTF-8 multi-byte sequence).
  Otherwise, it's the first byte of a multi-byte sequence and the number of leading 1 bits indicates how many bytes there are in total for
this sequence (110... means two bytes, 1110... means three bytes, etc).
*/

NS_ALWAYS_INLINE bool nsUnicodeUtils::IsUtf8StartByte(char iByte)
{
  // valid utf8 start bytes are 0x0-------, 0x110-----, 0x1110----, 0x11110---, etc
  return ((iByte & 0x80) == 0) || ((iByte & 0xE0) == 0xC0) || ((iByte & 0xF0) == 0xE0) || ((iByte & 0xF8) == 0xF0) || ((iByte & 0xFC) == 0xF8);
}

NS_ALWAYS_INLINE bool nsUnicodeUtils::IsUtf8ContinuationByte(char iByte)
{
  // check whether the two upper bits are set to '10'
  return (iByte & 0xC0) == 0x80;
}

NS_ALWAYS_INLINE bool nsUnicodeUtils::IsASCII(nsUInt32 uiChar)
{
  return (uiChar <= 127);
}

inline nsUInt32 nsUnicodeUtils::GetUtf8SequenceLength(char iFirstByte)
{
  const nsUInt32 uiBit7 = iFirstByte & NS_BIT(7);
  const nsUInt32 uiBit6 = iFirstByte & NS_BIT(6);
  const nsUInt32 uiBit5 = iFirstByte & NS_BIT(5);
  const nsUInt32 uiBit4 = iFirstByte & NS_BIT(4);

  if (uiBit7 == 0) // ASCII character '0xxxxxxx'
    return 1;

  NS_IGNORE_UNUSED(uiBit6);
  NS_ASSERT_DEV(uiBit6 != 0, "Invalid Leading UTF-8 Byte.");

  if (uiBit5 == 0) // '110xxxxx'
    return 2;
  if (uiBit4 == 0) // '1110xxxx'
    return 3;

  // '1111xxxx'
  return 4;
}

template <typename ByteIterator>
nsUInt32 nsUnicodeUtils::DecodeUtf8ToUtf32(ByteIterator& ref_szUtf8Iterator)
{
  return utf8::unchecked::next(ref_szUtf8Iterator);
}

template <typename UInt16Iterator>
bool nsUnicodeUtils::IsUtf16Surrogate(UInt16Iterator& ref_szUtf16Iterator)
{
  uint32_t cp = utf8::internal::mask16(*ref_szUtf16Iterator);
  return utf8::internal::is_lead_surrogate(cp);
}

template <typename UInt16Iterator>
nsUInt32 nsUnicodeUtils::DecodeUtf16ToUtf32(UInt16Iterator& ref_szUtf16Iterator)
{
  uint32_t cp = utf8::internal::mask16(*ref_szUtf16Iterator++);
  if (utf8::internal::is_lead_surrogate(cp))
  {
    uint32_t trail_surrogate = utf8::internal::mask16(*ref_szUtf16Iterator++);
    cp = (cp << 10) + trail_surrogate + utf8::internal::SURROGATE_OFFSET;
  }

  return cp;
}

template <typename WCharIterator>
nsUInt32 nsUnicodeUtils::DecodeWCharToUtf32(WCharIterator& ref_szWCharIterator)
{
  if (sizeof(wchar_t) == 2)
  {
    return DecodeUtf16ToUtf32(ref_szWCharIterator);
  }
  else // sizeof(wchar_t) == 4
  {
    const nsUInt32 uiResult = *ref_szWCharIterator;
    ++ref_szWCharIterator;
    return uiResult;
  }
}

template <typename ByteIterator>
void nsUnicodeUtils::EncodeUtf32ToUtf8(nsUInt32 uiUtf32, ByteIterator& ref_szUtf8Output)
{
  ref_szUtf8Output = utf8::unchecked::utf32to8(&uiUtf32, &uiUtf32 + 1, ref_szUtf8Output);
}

template <typename UInt16Iterator>
void nsUnicodeUtils::EncodeUtf32ToUtf16(nsUInt32 uiUtf32, UInt16Iterator& ref_szUtf16Output)
{
  if (uiUtf32 > 0xffff)
  {
    // make a surrogate pair
    *ref_szUtf16Output++ = static_cast<uint16_t>((uiUtf32 >> 10) + utf8::internal::LEAD_OFFSET);
    *ref_szUtf16Output++ = static_cast<uint16_t>((uiUtf32 & 0x3ff) + utf8::internal::TRAIL_SURROGATE_MIN);
  }
  else
    *ref_szUtf16Output++ = static_cast<uint16_t>(uiUtf32);
}

template <typename WCharIterator>
void nsUnicodeUtils::EncodeUtf32ToWChar(nsUInt32 uiUtf32, WCharIterator& ref_szWCharOutput)
{
  if (sizeof(wchar_t) == 2)
  {
    EncodeUtf32ToUtf16(uiUtf32, ref_szWCharOutput);
  }
  else
  {
    *ref_szWCharOutput = static_cast<wchar_t>(uiUtf32);
    ++ref_szWCharOutput;
  }
}

inline nsUInt32 nsUnicodeUtils::ConvertUtf8ToUtf32(const char* pFirstChar)
{
  return utf8::unchecked::peek_next(pFirstChar);
}

inline nsUInt32 nsUnicodeUtils::GetSizeForCharacterInUtf8(nsUInt32 uiCharacter)
{
  // Basically implements this: http://en.wikipedia.org/wiki/Utf8#Description

  if (uiCharacter <= 0x0000007f)
    return 1;

  if (uiCharacter <= 0x000007ff)
    return 2;

  if (uiCharacter <= 0x0000ffff)
    return 3;

  // UTF-8 can use up to 6 bytes to encode a code point
  // however some committee agreed that never more than 4 bytes are used (no need for more than 21 Bits)
  // this implementation assumes in several places, that the UTF-8 encoding never uses more than 4 bytes

  NS_ASSERT_DEV(uiCharacter <= 0x0010ffff, "Invalid Unicode Codepoint");
  return 4;
}

inline bool nsUnicodeUtils::IsValidUtf8(const char* szString, const char* szStringEnd)
{
  if (szStringEnd == GetMaxStringEnd<char>())
    szStringEnd = szString + strlen(szString);

  return utf8::is_valid(szString, szStringEnd);
}

inline bool nsUnicodeUtils::SkipUtf8Bom(const char*& ref_szUtf8)
{
  NS_ASSERT_DEBUG(ref_szUtf8 != nullptr, "This function expects non nullptr pointers");

  if (utf8::starts_with_bom(ref_szUtf8, ref_szUtf8 + 4))
  {
    ref_szUtf8 += 3;
    return true;
  }

  return false;
}

inline bool nsUnicodeUtils::SkipUtf16BomLE(const nsUInt16*& ref_pUtf16)
{
  NS_ASSERT_DEBUG(ref_pUtf16 != nullptr, "This function expects non nullptr pointers");

  if (*ref_pUtf16 == nsUnicodeUtils::Utf16BomLE)
  {
    ++ref_pUtf16;
    return true;
  }

  return false;
}

inline bool nsUnicodeUtils::SkipUtf16BomBE(const nsUInt16*& ref_pUtf16)
{
  NS_ASSERT_DEBUG(ref_pUtf16 != nullptr, "This function expects non nullptr pointers");

  if (*ref_pUtf16 == nsUnicodeUtils::Utf16BomBE)
  {
    ++ref_pUtf16;
    return true;
  }

  return false;
}

inline void nsUnicodeUtils::MoveToNextUtf8(const char*& ref_szUtf8, nsUInt32 uiNumCharacters)
{
  NS_ASSERT_DEBUG(ref_szUtf8 != nullptr, "Bad programmer!");

  while (uiNumCharacters > 0)
  {
    NS_ASSERT_DEV(*ref_szUtf8 != '\0', "The given string must not point to the zero terminator.");

    do
    {
      ++ref_szUtf8;
    } while (IsUtf8ContinuationByte(*ref_szUtf8));

    --uiNumCharacters;
  }
}

inline void nsUnicodeUtils::MoveToNextUtf8(const char*& ref_szUtf8, const char* szUtf8End, nsUInt32 uiNumCharacters)
{
  NS_ASSERT_DEBUG(ref_szUtf8 != nullptr, "Bad programmer!");

  while (uiNumCharacters > 0 && ref_szUtf8 < szUtf8End)
  {
    NS_ASSERT_DEV(*ref_szUtf8 != '\0', "The given string must not point to the zero terminator.");

    do
    {
      ++ref_szUtf8;
    } while ((ref_szUtf8 < szUtf8End) && IsUtf8ContinuationByte(*ref_szUtf8));

    --uiNumCharacters;
  }
}

inline void nsUnicodeUtils::MoveToPriorUtf8(const char*& ref_szUtf8, nsUInt32 uiNumCharacters)
{
  NS_ASSERT_DEBUG(ref_szUtf8 != nullptr, "Bad programmer!");

  while (uiNumCharacters > 0)
  {
    do
    {
      --ref_szUtf8;
    } while (IsUtf8ContinuationByte(*ref_szUtf8));

    --uiNumCharacters;
  }
}
template <typename T>
constexpr T* nsUnicodeUtils::GetMaxStringEnd()
{
  return reinterpret_cast<T*>(-1);
}
