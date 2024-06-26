/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Basics.h>

/// \brief This class provides functions to do atomic operations.
///
/// Atomic operations are generally faster than mutexes, and should therefore be preferred whenever possible.
/// However only the operations in themselves are atomic, once you execute several of them in sequence,
/// the sequence will not be atomic.
/// Also atomic operations are a lot slower than non-atomic operations, thus you should not use them in code that
/// does not need to be thread-safe.
/// nsAtomicInteger is built on top of nsAtomicUtils and provides a more convenient interface to use atomic
/// integer instructions.
struct NS_FOUNDATION_DLL nsAtomicUtils
{
  /// \brief Returns src as an atomic operation and returns its value.
  static nsInt32 Read(volatile const nsInt32& iSrc); // [tested]

  /// \brief Returns src as an atomic operation and returns its value.
  static nsInt64 Read(volatile const nsInt64& iSrc); // [tested]

  /// \brief Increments dest as an atomic operation and returns the new value.
  static nsInt32 Increment(volatile nsInt32& ref_iDest); // [tested]

  /// \brief Increments dest as an atomic operation and returns the new value.
  static nsInt64 Increment(volatile nsInt64& ref_iDest); // [tested]

  /// \brief Decrements dest as an atomic operation and returns the new value.
  static nsInt32 Decrement(volatile nsInt32& ref_iDest); // [tested]

  /// \brief Decrements dest as an atomic operation and returns the new value.
  static nsInt64 Decrement(volatile nsInt64& ref_iDest); // [tested]

  /// \brief Increments dest as an atomic operation and returns the old value.
  static nsInt32 PostIncrement(volatile nsInt32& ref_iDest); // [tested]

  /// \brief Increments dest as an atomic operation and returns the old value.
  static nsInt64 PostIncrement(volatile nsInt64& ref_iDest); // [tested]

  /// \brief Decrements dest as an atomic operation and returns the old value.
  static nsInt32 PostDecrement(volatile nsInt32& ref_iDest); // [tested]

  /// \brief Decrements dest as an atomic operation and returns the old value.
  static nsInt64 PostDecrement(volatile nsInt64& ref_iDest); // [tested]

  /// \brief Adds value to dest as an atomic operation.
  static void Add(volatile nsInt32& ref_iDest, nsInt32 value); // [tested]

  /// \brief Adds value to dest as an atomic operation.
  static void Add(volatile nsInt64& ref_iDest, nsInt64 value); // [tested]

  /// \brief Performs an atomic bitwise AND on dest using value.
  static void And(volatile nsInt32& ref_iDest, nsInt32 value); // [tested]

  /// \brief Performs an atomic bitwise AND on dest using value.
  static void And(volatile nsInt64& ref_iDest, nsInt64 value); // [tested]

  /// \brief Performs an atomic bitwise OR on dest using value.
  static void Or(volatile nsInt32& ref_iDest, nsInt32 value); // [tested]

  /// \brief Performs an atomic bitwise OR on dest using value.
  static void Or(volatile nsInt64& ref_iDest, nsInt64 value); // [tested]

  /// \brief Performs an atomic bitwise XOR on dest using value.
  static void Xor(volatile nsInt32& ref_iDest, nsInt32 value); // [tested]

  /// \brief Performs an atomic bitwise XOR on dest using value.
  static void Xor(volatile nsInt64& ref_iDest, nsInt64 value); // [tested]

  /// \brief Performs an atomic min operation on dest using value.
  static void Min(volatile nsInt32& ref_iDest, nsInt32 value); // [tested]

  /// \brief Performs an atomic min operation on dest using value.
  static void Min(volatile nsInt64& ref_iDest, nsInt64 value); // [tested]

  /// \brief Performs an atomic max operation on dest using value.
  static void Max(volatile nsInt32& ref_iDest, nsInt32 value); // [tested]

  /// \brief Performs an atomic max operation on dest using value.
  static void Max(volatile nsInt64& ref_iDest, nsInt64 value); // [tested]

  /// \brief Sets dest to value as an atomic operation and returns the original value of dest.
  static nsInt32 Set(volatile nsInt32& ref_iDest, nsInt32 value); // [tested]

  /// \brief Sets dest to value as an atomic operation and returns the original value of dest.
  static nsInt64 Set(volatile nsInt64& ref_iDest, nsInt64 value); // [tested]

  /// \brief If *dest* is equal to *expected*, this function sets *dest* to *value* and returns true. Otherwise *dest* will not be modified and the
  /// function returns false.
  static bool TestAndSet(volatile nsInt32& ref_iDest, nsInt32 iExpected, nsInt32 value); // [tested]

  /// \brief If *dest* is equal to *expected*, this function sets *dest* to *value* and returns true. Otherwise *dest* will not be modified and the
  /// function returns false.
  static bool TestAndSet(volatile nsInt64& ref_iDest, nsInt64 iExpected, nsInt64 value); // [tested]

  /// \brief If *dest* is equal to *expected*, this function sets *dest* to *value* and returns true. Otherwise *dest* will not be modified and the
  /// function returns false.
  static bool TestAndSet(void** volatile pDest, void* pExpected, void* value); // [tested]

  /// \brief If *dest* is equal to *expected*, this function sets *dest* to *value*. Otherwise *dest* will not be modified. Always returns the value
  /// of *dest* before the modification.
  static nsInt32 CompareAndSwap(volatile nsInt32& ref_iDest, nsInt32 iExpected, nsInt32 value); // [tested]

  /// \brief If *dest* is equal to *expected*, this function sets *dest* to *value*. Otherwise *dest* will not be modified. Always returns the value
  /// of *dest* before the modification.
  static nsInt64 CompareAndSwap(volatile nsInt64& ref_iDest, nsInt64 iExpected, nsInt64 value); // [tested]
};

// Include inline file
#if NS_ENABLED(NS_PLATFORM_WINDOWS)
#  include <Foundation/Threading/Implementation/Win/AtomicUtils_win.h>
#elif NS_ENABLED(NS_PLATFORM_OSX) || NS_ENABLED(NS_PLATFORM_LINUX) || NS_ENABLED(NS_PLATFORM_ANDROID)
#  include <Foundation/Threading/Implementation/Posix/AtomicUtils_posix.h>
#else
#  error "Atomics are not implemented on current platform"
#endif
