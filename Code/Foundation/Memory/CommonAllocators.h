/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */

#pragma once

/// \file

#include <Foundation/Memory/Allocator.h>

#include <Foundation/Memory/Policies/AlignedHeapAllocation.h>
#include <Foundation/Memory/Policies/GuardedAllocation.h>
#include <Foundation/Memory/Policies/HeapAllocation.h>
#include <Foundation/Memory/Policies/ProxyAllocation.h>


/// \brief Default heap allocator
using nsAlignedHeapAllocator = nsAllocator<nsMemoryPolicies::nsAlignedHeapAllocation>;

/// \brief Default heap allocator
using nsHeapAllocator = nsAllocator<nsMemoryPolicies::nsHeapAllocation>;

/// \brief Guarded allocator
using nsGuardedAllocator = nsAllocator<nsMemoryPolicies::nsGuardedAllocation>;

/// \brief Proxy allocator
using nsProxyAllocator = nsAllocator<nsMemoryPolicies::nsProxyAllocation>;
