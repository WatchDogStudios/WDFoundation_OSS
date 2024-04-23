/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Basics.h>

// Deactivate Doxygen document generation for the following block.
/// \cond

struct nsOSFileData
{
  nsOSFileData() { m_pFileHandle = nullptr; }

  FILE* m_pFileHandle;
};

#if NS_ENABLED(NS_SUPPORTS_FILE_ITERATORS)

struct nsFileIterationData
{
  // This is storing DIR*, which we can't forward declare
  nsHybridArray<void*, 16> m_Handles;
  nsString m_wildcardSearch;
};

#endif

/// \endcond
