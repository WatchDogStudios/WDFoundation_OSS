/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Types/TypeTraits.h>

#include <Foundation/Threading/AtomicUtils.h>

template <int T>
struct nsAtomicStorageType
{
};

template <>
struct nsAtomicStorageType<0>
{
  using Type = nsInt32;
};

template <>
struct nsAtomicStorageType<1>
{
  using Type = nsInt64;
};

/// \brief Integer class that can be manipulated in an atomic (i.e. thread-safe) fashion.
template <typename T>
class nsAtomicInteger
{
  using UnderlyingType = typename nsAtomicStorageType<sizeof(T) / 32>::Type;

public:
  NS_DECLARE_POD_TYPE();

  /// \brief Initializes the value to zero.
  nsAtomicInteger(); // [tested]

  /// \brief Initializes the object with a value
  nsAtomicInteger(const T value); // [tested]

  /// \brief Copy-constructor
  nsAtomicInteger(const nsAtomicInteger<T>& value); // [tested]

  /// \brief Assigns a new integer value to this object
  nsAtomicInteger& operator=(T value); // [tested]

  /// \brief Assignment operator
  nsAtomicInteger& operator=(const nsAtomicInteger& value); // [tested]

  /// \brief Increments the internal value and returns the incremented value
  T Increment(); // [tested]

  /// \brief Decrements the internal value and returns the decremented value
  T Decrement(); // [tested]

  /// \brief Increments the internal value and returns the value immediately before the increment
  T PostIncrement(); // [tested]

  /// \brief Decrements the internal value and returns the value immediately before the decrement
  T PostDecrement(); // [tested]

  void Add(T x);      // [tested]
  void Subtract(T x); // [tested]

  void And(T x); // [tested]
  void Or(T x);  // [tested]
  void Xor(T x); // [tested]

  void Min(T x); // [tested]
  void Max(T x); // [tested]

  /// \brief Sets the internal value to x and returns the original internal value.
  T Set(T x); // [tested]

  /// \brief Sets the internal value to x if the internal value is equal to expected and returns true, otherwise does nothing and returns false.
  bool TestAndSet(T expected, T x); // [tested]

  /// \brief If this is equal to *expected*, it is set to *value*. Otherwise it won't be modified. Always returns the previous value of this before
  /// the modification.
  T CompareAndSwap(T expected, T x); // [tested]

  operator T() const; // [tested]

private:
  volatile UnderlyingType m_Value;
};

/// \brief An atomic boolean variable. This is just a wrapper around an atomic int32 for convenience.
class nsAtomicBool
{
public:
  /// \brief Initializes the bool to 'false'.
  nsAtomicBool(); // [tested]
  ~nsAtomicBool();

  /// \brief Initializes the object with a value
  nsAtomicBool(bool value); // [tested]

  /// \brief Copy-constructor
  nsAtomicBool(const nsAtomicBool& rhs);

  /// \brief Sets the bool to the given value and returns its previous value.
  bool Set(bool value); // [tested]

  /// \brief Sets the bool to the given value.
  void operator=(bool value); // [tested]

  /// \brief Sets the bool to the given value.
  void operator=(const nsAtomicBool& rhs);

  /// \brief Returns the current value.
  operator bool() const; // [tested]

  /// \brief Sets the internal value to \a newValue if the internal value is equal to \a expected and returns true, otherwise does nothing and returns
  /// false.
  bool TestAndSet(bool bExpected, bool bNewValue);

private:
  nsAtomicInteger<nsInt32> m_iAtomicInt;
};

// Include inline file
#include <Foundation/Threading/Implementation/AtomicInteger_inl.h>

using nsAtomicInteger32 = nsAtomicInteger<nsInt32>; // [tested]
using nsAtomicInteger64 = nsAtomicInteger<nsInt64>; // [tested]
