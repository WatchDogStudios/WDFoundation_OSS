/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Strings/String.h>

NS_CREATE_SIMPLE_TEST(Strings, PathUtils)
{
  NS_TEST_BLOCK(nsTestBlock::Enabled, "IsPathSeparator")
  {
    for (int i = 0; i < 0xFFFF; ++i)
    {
      if (i == '/')
      {
        NS_TEST_BOOL(nsPathUtils::IsPathSeparator(i));
      }
      else if (i == '\\')
      {
        NS_TEST_BOOL(nsPathUtils::IsPathSeparator(i));
      }
      else
      {
        NS_TEST_BOOL(!nsPathUtils::IsPathSeparator(i));
      }
    }
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "FindPreviousSeparator")
  {
    const char* szPath = "This/Is\\My//Path.dot\\file.extension";

    NS_TEST_BOOL(nsPathUtils::FindPreviousSeparator(szPath, szPath + 35) == szPath + 20);
    NS_TEST_BOOL(nsPathUtils::FindPreviousSeparator(szPath, szPath + 20) == szPath + 11);
    NS_TEST_BOOL(nsPathUtils::FindPreviousSeparator(szPath, szPath + 11) == szPath + 10);
    NS_TEST_BOOL(nsPathUtils::FindPreviousSeparator(szPath, szPath + 10) == szPath + 7);
    NS_TEST_BOOL(nsPathUtils::FindPreviousSeparator(szPath, szPath + 7) == szPath + 4);
    NS_TEST_BOOL(nsPathUtils::FindPreviousSeparator(szPath, szPath + 4) == nullptr);
    NS_TEST_BOOL(nsPathUtils::FindPreviousSeparator(szPath, szPath) == nullptr);
    NS_TEST_BOOL(nsPathUtils::FindPreviousSeparator(nullptr, nullptr) == nullptr);
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "HasAnyExtension")
  {
    NS_TEST_BOOL(nsPathUtils::HasAnyExtension("This/Is\\My//Path.dot\\file.extension"));
    NS_TEST_BOOL(!nsPathUtils::HasAnyExtension("This/Is\\My//Path.dot\\file_no_extension"));
    NS_TEST_BOOL(!nsPathUtils::HasAnyExtension(""));
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "HasExtension")
  {
    NS_TEST_BOOL(nsPathUtils::HasExtension("This/Is\\My//Path.dot\\file.extension", ".Extension"));
    NS_TEST_BOOL(nsPathUtils::HasExtension("This/Is\\My//Path.dot\\file.ext", "EXT"));
    NS_TEST_BOOL(!nsPathUtils::HasExtension("This/Is\\My//Path.dot\\file.ext", "NEXT"));
    NS_TEST_BOOL(!nsPathUtils::HasExtension("This/Is\\My//Path.dot\\file.extension", ".Ext"));
    NS_TEST_BOOL(!nsPathUtils::HasExtension("This/Is\\My//Path.dot\\file.extension", "sion"));
    NS_TEST_BOOL(!nsPathUtils::HasExtension("", "ext"));
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "GetFileExtension")
  {
    NS_TEST_BOOL(nsPathUtils::GetFileExtension("This/Is\\My//Path.dot\\file.extension") == "extension");
    NS_TEST_BOOL(nsPathUtils::GetFileExtension("This/Is\\My//Path.dot\\file") == "");
    NS_TEST_BOOL(nsPathUtils::GetFileExtension("") == "");
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "GetFileNameAndExtension")
  {
    NS_TEST_BOOL(nsPathUtils::GetFileNameAndExtension("This/Is\\My//Path.dot\\file.extension") == "file.extension");
    NS_TEST_BOOL(nsPathUtils::GetFileNameAndExtension("This/Is\\My//Path.dot\\.extension") == ".extension");
    NS_TEST_BOOL(nsPathUtils::GetFileNameAndExtension("This/Is\\My//Path.dot\\file") == "file");
    NS_TEST_BOOL(nsPathUtils::GetFileNameAndExtension("\\file") == "file");
    NS_TEST_BOOL(nsPathUtils::GetFileNameAndExtension("") == "");
    NS_TEST_BOOL(nsPathUtils::GetFileNameAndExtension("/") == "");
    NS_TEST_BOOL(nsPathUtils::GetFileNameAndExtension("This/Is\\My//Path.dot\\") == "");
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "GetFileName")
  {
    NS_TEST_BOOL(nsPathUtils::GetFileName("This/Is\\My//Path.dot\\file.extension") == "file");
    NS_TEST_BOOL(nsPathUtils::GetFileName("This/Is\\My//Path.dot\\file") == "file");
    NS_TEST_BOOL(nsPathUtils::GetFileName("\\file") == "file");
    NS_TEST_BOOL(nsPathUtils::GetFileName("") == "");
    NS_TEST_BOOL(nsPathUtils::GetFileName("/") == "");
    NS_TEST_BOOL(nsPathUtils::GetFileName("This/Is\\My//Path.dot\\") == "");

    // so far we treat file and folders whose names start with a '.' as extensions
    NS_TEST_BOOL(nsPathUtils::GetFileName("This/Is\\My//Path.dot\\.stupidfile") == "");
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "GetFileDirectory")
  {
    NS_TEST_BOOL(nsPathUtils::GetFileDirectory("This/Is\\My//Path.dot\\file.extension") == "This/Is\\My//Path.dot\\");
    NS_TEST_BOOL(nsPathUtils::GetFileDirectory("This/Is\\My//Path.dot\\.extension") == "This/Is\\My//Path.dot\\");
    NS_TEST_BOOL(nsPathUtils::GetFileDirectory("This/Is\\My//Path.dot\\file") == "This/Is\\My//Path.dot\\");
    NS_TEST_BOOL(nsPathUtils::GetFileDirectory("\\file") == "\\");
    NS_TEST_BOOL(nsPathUtils::GetFileDirectory("") == "");
    NS_TEST_BOOL(nsPathUtils::GetFileDirectory("/") == "/");
    NS_TEST_BOOL(nsPathUtils::GetFileDirectory("This/Is\\My//Path.dot\\") == "This/Is\\My//Path.dot\\");
    NS_TEST_BOOL(nsPathUtils::GetFileDirectory("This") == "");
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "IsAbsolutePath")
  {
#if NS_ENABLED(NS_PLATFORM_WINDOWS)
    NS_TEST_BOOL(nsPathUtils::IsAbsolutePath("C:\\temp.stuff"));
    NS_TEST_BOOL(nsPathUtils::IsAbsolutePath("C:/temp.stuff"));
    NS_TEST_BOOL(nsPathUtils::IsAbsolutePath("\\\\myserver\\temp.stuff"));
    NS_TEST_BOOL(!nsPathUtils::IsAbsolutePath("\\myserver\\temp.stuff"));
    NS_TEST_BOOL(!nsPathUtils::IsAbsolutePath("temp.stuff"));
    NS_TEST_BOOL(!nsPathUtils::IsAbsolutePath("/temp.stuff"));
    NS_TEST_BOOL(!nsPathUtils::IsAbsolutePath("\\temp.stuff"));
    NS_TEST_BOOL(!nsPathUtils::IsAbsolutePath("..\\temp.stuff"));
    NS_TEST_BOOL(!nsPathUtils::IsAbsolutePath(".\\temp.stuff"));
#elif NS_ENABLED(NS_PLATFORM_OSX) || NS_ENABLED(NS_PLATFORM_LINUX) || NS_ENABLED(NS_PLATFORM_ANDROID)
    NS_TEST_BOOL(nsPathUtils::IsAbsolutePath("/usr/local/.stuff"));
    NS_TEST_BOOL(nsPathUtils::IsAbsolutePath("/file.test"));
    NS_TEST_BOOL(!nsPathUtils::IsAbsolutePath("./file.stuff"));
    NS_TEST_BOOL(!nsPathUtils::IsAbsolutePath("file.stuff"));
#else
#  error "Unknown platform."
#endif
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "GetRootedPathParts")
  {
    nsStringView root, relPath;
    nsPathUtils::GetRootedPathParts(":MyRoot\\folder\\file.txt", root, relPath);
    NS_TEST_BOOL(nsPathUtils::GetRootedPathRootName(":MyRoot\\folder\\file.txt") == root);
    NS_TEST_BOOL(root == "MyRoot");
    NS_TEST_BOOL(relPath == "folder\\file.txt");
    nsPathUtils::GetRootedPathParts("folder\\file2.txt", root, relPath);
    NS_TEST_BOOL(root.IsEmpty());
    NS_TEST_BOOL(relPath == "folder\\file2.txt");
  }
}
