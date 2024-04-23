/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

/// \file

/// Global settings for how to to compile wdstdcorelib.
/// Modify these settings as you needed in your project.


#ifdef BUILDSYSTEM_COMPILE_ENGINE_AS_DLL
#  undef NS_COMPILE_ENGINE_AS_DLL
#  define NS_COMPILE_ENGINE_AS_DLL NS_ON
#else
#  undef NS_COMPILE_ENGINE_AS_DLL
#  define NS_COMPILE_ENGINE_AS_DLL NS_OFF
#endif

#if defined(BUILDSYSTEM_BUILDTYPE_Shipping)

// Development checks like assert.
#  undef NS_COMPILE_FOR_DEVELOPMENT
#  define NS_COMPILE_FOR_DEVELOPMENT NS_OFF

// Performance profiling features
#  undef NS_USE_PROFILING
#  define NS_USE_PROFILING NS_OFF

// Tracking of memory allocations.
#  undef NS_USE_ALLOCATION_TRACKING
#  define NS_USE_ALLOCATION_TRACKING NS_OFF

// Stack traces for memory allocations.
#  undef NS_USE_ALLOCATION_STACK_TRACING
#  define NS_USE_ALLOCATION_STACK_TRACING NS_OFF

#else

// Development checks like assert.
#  undef NS_COMPILE_FOR_DEVELOPMENT
#  define NS_COMPILE_FOR_DEVELOPMENT NS_ON

// Performance profiling features
#  undef NS_USE_PROFILING
#  define NS_USE_PROFILING NS_ON

// Tracking of memory allocations.
#  undef NS_USE_ALLOCATION_TRACKING
#  define NS_USE_ALLOCATION_TRACKING NS_ON

// Stack traces for memory allocations.
#  undef NS_USE_ALLOCATION_STACK_TRACING
#  define NS_USE_ALLOCATION_STACK_TRACING NS_ON

#endif
/// Whether to enable the console optimizations to increase performance on console platforms, and improve interation times when working with development kits. (DISABLED ON OPEN-SOURCE RELEASE)
#define NS_ENABLECONSOLE_OPTIMIZATIONS NS_OFF
/// Whether game objects compute and store their velocity since the last frame (increases object size)
#define NS_GAMEOBJECT_VELOCITY NS_ON

// Migration code path. Added in March 2023, should be removed after a 'save' time.
#undef NS_MIGRATE_RUNTIMECONFIGS
#define NS_MIGRATE_RUNTIMECONFIGS NS_ON
