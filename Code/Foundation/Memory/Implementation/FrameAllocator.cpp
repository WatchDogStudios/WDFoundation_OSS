/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <Foundation/FoundationPCH.h>

#include <Foundation/Configuration/Startup.h>
#include <Foundation/Memory/FrameAllocator.h>
#include <Foundation/Profiling/Profiling.h>
#include <Foundation/Strings/StringBuilder.h>

nsDoubleBufferedStackAllocator::nsDoubleBufferedStackAllocator(nsStringView sName0, nsAllocatorBase* pParent)
{
  nsStringBuilder sName = sName0;
  sName.Append("0");

  m_pCurrentAllocator = NS_DEFAULT_NEW(StackAllocatorType, sName, pParent);

  sName = sName0;
  sName.Append("1");

  m_pOtherAllocator = NS_DEFAULT_NEW(StackAllocatorType, sName, pParent);
}

nsDoubleBufferedStackAllocator::~nsDoubleBufferedStackAllocator()
{
  NS_DEFAULT_DELETE(m_pCurrentAllocator);
  NS_DEFAULT_DELETE(m_pOtherAllocator);
}

void nsDoubleBufferedStackAllocator::Swap()
{
  nsMath::Swap(m_pCurrentAllocator, m_pOtherAllocator);

  m_pCurrentAllocator->Reset();
}

void nsDoubleBufferedStackAllocator::Reset()
{
  m_pCurrentAllocator->Reset();
  m_pOtherAllocator->Reset();
}


// clang-format off
NS_BEGIN_SUBSYSTEM_DECLARATION(Foundation, FrameAllocator)

  ON_CORESYSTEMS_STARTUP
  {
    nsFrameAllocator::Startup();
  }

  ON_CORESYSTEMS_SHUTDOWN
  {
    nsFrameAllocator::Shutdown();
  }

NS_END_SUBSYSTEM_DECLARATION;
// clang-format on

nsDoubleBufferedStackAllocator* nsFrameAllocator::s_pAllocator;

// static
void nsFrameAllocator::Swap()
{
  NS_PROFILE_SCOPE("FrameAllocator.Swap");

  s_pAllocator->Swap();
}

// static
void nsFrameAllocator::Reset()
{
  if (s_pAllocator)
  {
    s_pAllocator->Reset();
  }
}

// static
void nsFrameAllocator::Startup()
{
  s_pAllocator = NS_DEFAULT_NEW(nsDoubleBufferedStackAllocator, "FrameAllocator", nsFoundation::GetAlignedAllocator());
}

// static
void nsFrameAllocator::Shutdown()
{
  NS_DEFAULT_DELETE(s_pAllocator);
}

NS_STATICLINK_FILE(Foundation, Foundation_Memory_Implementation_FrameAllocator);
