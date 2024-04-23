/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Strings/Implementation/StringIterator.h>
#include <Foundation/Strings/StringBuilder.h>

const char* nsPathUtils::FindPreviousSeparator(const char* szPathStart, const char* szStartSearchAt)
{
  if (nsStringUtils::IsNullOrEmpty(szPathStart))
    return nullptr;

  while (szStartSearchAt > szPathStart)
  {
    nsUnicodeUtils::MoveToPriorUtf8(szStartSearchAt);

    if (IsPathSeparator(*szStartSearchAt))
      return szStartSearchAt;
  }

  return nullptr;
}

bool nsPathUtils::HasAnyExtension(nsStringView sPath)
{
  const char* szDot = nsStringUtils::FindLastSubString(sPath.GetStartPointer(), ".", nullptr, sPath.GetEndPointer());

  if (szDot == nullptr)
    return false;

  // find the last separator in the string
  const char* szSeparator = FindPreviousSeparator(sPath.GetStartPointer(), sPath.GetEndPointer());

  return (szSeparator < szDot);
}

bool nsPathUtils::HasExtension(nsStringView sPath, nsStringView sExtension)
{
  if (nsStringUtils::StartsWith(sExtension.GetStartPointer(), ".", sExtension.GetEndPointer()))
    return nsStringUtils::EndsWith_NoCase(sPath.GetStartPointer(), sExtension.GetStartPointer(), sPath.GetEndPointer(), sExtension.GetEndPointer());

  nsStringBuilder sExt;
  sExt.Append(".", sExtension);

  return nsStringUtils::EndsWith_NoCase(sPath.GetStartPointer(), sExt.GetData(), sPath.GetEndPointer());
}

nsStringView nsPathUtils::GetFileExtension(nsStringView sPath)
{
  const char* szDot = nsStringUtils::FindLastSubString(sPath.GetStartPointer(), ".", nullptr, sPath.GetEndPointer());

  if (szDot == nullptr)
    return nsStringView(nullptr);

  // find the last separator in the string
  const char* szSeparator = FindPreviousSeparator(sPath.GetStartPointer(), sPath.GetEndPointer());

  if (szSeparator > szDot)
    return nsStringView(nullptr);

  return nsStringView(szDot + 1, sPath.GetEndPointer());
}

nsStringView nsPathUtils::GetFileNameAndExtension(nsStringView sPath)
{
  const char* szSeparator = FindPreviousSeparator(sPath.GetStartPointer(), sPath.GetEndPointer());

  if (szSeparator == nullptr)
    return sPath;

  return nsStringView(szSeparator + 1, sPath.GetEndPointer());
}

nsStringView nsPathUtils::GetFileName(nsStringView sPath)
{
  const char* szSeparator = FindPreviousSeparator(sPath.GetStartPointer(), sPath.GetEndPointer());

  const char* szDot = nsStringUtils::FindLastSubString(sPath.GetStartPointer(), ".", sPath.GetEndPointer());

  if (szDot < szSeparator) // includes (szDot == nullptr), szSeparator will never be nullptr here -> no extension
  {
    return nsStringView(szSeparator + 1, sPath.GetEndPointer());
  }

  if (szSeparator == nullptr)
  {
    if (szDot == nullptr) // no folder, no extension -> the entire thing is just a name
      return sPath;

    return nsStringView(sPath.GetStartPointer(), szDot); // no folder, but an extension -> remove the extension
  }

  // now: there is a separator AND an extension

  return nsStringView(szSeparator + 1, szDot);
}

nsStringView nsPathUtils::GetFileDirectory(nsStringView sPath)
{
  auto it = rbegin(sPath);

  // if it already ends in a path separator, do not return a different directory
  if (IsPathSeparator(it.GetCharacter()))
    return sPath;

  // find the last separator in the string
  const char* szSeparator = FindPreviousSeparator(sPath.GetStartPointer(), sPath.GetEndPointer());

  // no path separator -> root dir -> return the empty path
  if (szSeparator == nullptr)
    return nsStringView(nullptr);

  return nsStringView(sPath.GetStartPointer(), szSeparator + 1);
}

#if NS_ENABLED(NS_PLATFORM_WINDOWS)
const char nsPathUtils::OsSpecificPathSeparator = '\\';
#elif NS_ENABLED(NS_PLATFORM_LINUX) || NS_ENABLED(NS_PLATFORM_ANDROID)
const char nsPathUtils::OsSpecificPathSeparator = '/';
#elif NS_ENABLED(NS_PLATFORM_OSX)
const char nsPathUtils::OsSpecificPathSeparator = '/';
#else
#  error "Unknown platform."
#endif

bool nsPathUtils::IsAbsolutePath(nsStringView sPath)
{
  if (sPath.GetElementCount() < 2)
    return false;

  const char* szPath = sPath.GetStartPointer();

  // szPath[0] will not be \0 -> so we can access szPath[1] without problems

#if NS_ENABLED(NS_PLATFORM_WINDOWS)
  /// if it is an absolute path, character 0 must be ASCII (A - Z)
  /// checks for local paths, i.e. 'C:\stuff' and UNC paths, i.e. '\\server\stuff'
  /// not sure if we should handle '//' identical to '\\' (currently we do)
  return ((szPath[1] == ':') || (IsPathSeparator(szPath[0]) && IsPathSeparator(szPath[1])));
#elif NS_ENABLED(NS_PLATFORM_LINUX) || NS_ENABLED(NS_PLATFORM_ANDROID)
  return (szPath[0] == '/');
#elif NS_ENABLED(NS_PLATFORM_OSX)
  return (szPath[0] == '/');
#else
#  error "Unknown platform."
#endif
}

bool nsPathUtils::IsRelativePath(nsStringView sPath)
{
  if (sPath.IsEmpty())
    return true;

  // if it starts with a separator, it is not a relative path, ever
  if (nsPathUtils::IsPathSeparator(*sPath.GetStartPointer()))
    return false;

  return !IsAbsolutePath(sPath) && !IsRootedPath(sPath);
}

bool nsPathUtils::IsRootedPath(nsStringView sPath)
{
  return !sPath.IsEmpty() && *sPath.GetStartPointer() == ':';
}

void nsPathUtils::GetRootedPathParts(nsStringView sPath, nsStringView& ref_sRoot, nsStringView& ref_sRelPath)
{
  ref_sRoot = nsStringView();
  ref_sRelPath = sPath;

  if (!IsRootedPath(sPath))
    return;

  const char* szStart = sPath.GetStartPointer();
  const char* szPathEnd = sPath.GetEndPointer();

  do
  {
    nsUnicodeUtils::MoveToNextUtf8(szStart, szPathEnd);

    if (*szStart == '\0')
      return;

  } while (IsPathSeparator(*szStart));

  const char* szEnd = szStart;
  nsUnicodeUtils::MoveToNextUtf8(szEnd, szPathEnd);

  while (*szEnd != '\0' && !IsPathSeparator(*szEnd))
    nsUnicodeUtils::MoveToNextUtf8(szEnd, szPathEnd);

  ref_sRoot = nsStringView(szStart, szEnd);
  if (*szEnd == '\0')
  {
    ref_sRelPath = nsStringView();
  }
  else
  {
    // skip path separator for the relative path
    nsUnicodeUtils::MoveToNextUtf8(szEnd, szPathEnd);
    ref_sRelPath = nsStringView(szEnd, szPathEnd);
  }
}

nsStringView nsPathUtils::GetRootedPathRootName(nsStringView sPath)
{
  nsStringView root, relPath;
  GetRootedPathParts(sPath, root, relPath);
  return root;
}

bool nsPathUtils::IsValidFilenameChar(nsUInt32 uiCharacter)
{
  /// \test Not tested yet

  // Windows: https://msdn.microsoft.com/library/windows/desktop/aa365247(v=vs.85).aspx
  // Unix: https://en.wikipedia.org/wiki/Filename#Reserved_characters_and_words
  // Details can be more complicated (there might be reserved names depending on the filesystem), but in general all platforms behave like
  // this:
  static const nsUInt32 forbiddenFilenameChars[] = {'<', '>', ':', '"', '|', '?', '*', '\\', '/', '\t', '\b', '\n', '\r', '\0'};

  for (int i = 0; i < NS_ARRAY_SIZE(forbiddenFilenameChars); ++i)
  {
    if (forbiddenFilenameChars[i] == uiCharacter)
      return false;
  }

  return true;
}

bool nsPathUtils::ContainsInvalidFilenameChars(nsStringView sPath)
{
  /// \test Not tested yet

  nsStringIterator it = sPath.GetIteratorFront();

  for (; it.IsValid(); ++it)
  {
    if (!IsValidFilenameChar(it.GetCharacter()))
      return true;
  }

  return false;
}

void nsPathUtils::MakeValidFilename(nsStringView sFilename, nsUInt32 uiReplacementCharacter, nsStringBuilder& out_sFilename)
{
  NS_ASSERT_DEBUG(IsValidFilenameChar(uiReplacementCharacter), "Given replacement character is not allowed for filenames.");

  out_sFilename.Clear();

  for (auto it = sFilename.GetIteratorFront(); it.IsValid(); ++it)
  {
    nsUInt32 currentChar = it.GetCharacter();

    if (IsValidFilenameChar(currentChar) == false)
      out_sFilename.Append(uiReplacementCharacter);
    else
      out_sFilename.Append(currentChar);
  }
}

bool nsPathUtils::IsSubPath(nsStringView sPrefixPath, nsStringView sFullPath)
{
  /// \test this is new

  nsStringBuilder tmp = sPrefixPath;
  tmp.MakeCleanPath();
  tmp.AppendPath("");

  return sFullPath.StartsWith_NoCase(tmp);
}

NS_STATICLINK_FILE(Foundation, Foundation_Strings_Implementation_PathUtils);
