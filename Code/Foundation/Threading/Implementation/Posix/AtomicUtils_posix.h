/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#ifdef NS_ATOMICUTLS_POSIX_INL_H_INCLUDED
#  error "This file must not be included twice."
#endif

#define NS_ATOMICUTLS_POSIX_INL_H_INCLUDED


#include <Foundation/Math/Math.h>

NS_ALWAYS_INLINE nsInt32 nsAtomicUtils::Read(volatile const nsInt32& src)
{
  return __sync_fetch_and_or(const_cast<volatile nsInt32*>(&src), 0);
}

NS_ALWAYS_INLINE nsInt64 nsAtomicUtils::Read(volatile const nsInt64& src)
{
  return __sync_fetch_and_or_8(const_cast<volatile nsInt64*>(&src), 0);
}

NS_ALWAYS_INLINE nsInt32 nsAtomicUtils::Increment(volatile nsInt32& dest)
{
  return __sync_add_and_fetch(&dest, 1);
}

NS_ALWAYS_INLINE nsInt64 nsAtomicUtils::Increment(volatile nsInt64& dest)
{
  return __sync_add_and_fetch_8(&dest, 1);
}


NS_ALWAYS_INLINE nsInt32 nsAtomicUtils::Decrement(volatile nsInt32& dest)
{
  return __sync_sub_and_fetch(&dest, 1);
}

NS_ALWAYS_INLINE nsInt64 nsAtomicUtils::Decrement(volatile nsInt64& dest)
{
  return __sync_sub_and_fetch_8(&dest, 1);
}

NS_ALWAYS_INLINE nsInt32 nsAtomicUtils::PostIncrement(volatile nsInt32& dest)
{
  return __sync_fetch_and_add(&dest, 1);
}

NS_ALWAYS_INLINE nsInt64 nsAtomicUtils::PostIncrement(volatile nsInt64& dest)
{
  return __sync_fetch_and_add_8(&dest, 1);
}


NS_ALWAYS_INLINE nsInt32 nsAtomicUtils::PostDecrement(volatile nsInt32& dest)
{
  return __sync_fetch_and_sub(&dest, 1);
}

NS_ALWAYS_INLINE nsInt64 nsAtomicUtils::PostDecrement(volatile nsInt64& dest)
{
  return __sync_fetch_and_sub_8(&dest, 1);
}

NS_ALWAYS_INLINE void nsAtomicUtils::Add(volatile nsInt32& dest, nsInt32 value)
{
  __sync_fetch_and_add(&dest, value);
}

NS_ALWAYS_INLINE void nsAtomicUtils::Add(volatile nsInt64& dest, nsInt64 value)
{
  __sync_fetch_and_add_8(&dest, value);
}


NS_ALWAYS_INLINE void nsAtomicUtils::And(volatile nsInt32& dest, nsInt32 value)
{
  __sync_fetch_and_and(&dest, value);
}

NS_ALWAYS_INLINE void nsAtomicUtils::And(volatile nsInt64& dest, nsInt64 value)
{
  __sync_fetch_and_and_8(&dest, value);
}


NS_ALWAYS_INLINE void nsAtomicUtils::Or(volatile nsInt32& dest, nsInt32 value)
{
  __sync_fetch_and_or(&dest, value);
}

NS_ALWAYS_INLINE void nsAtomicUtils::Or(volatile nsInt64& dest, nsInt64 value)
{
  __sync_fetch_and_or_8(&dest, value);
}


NS_ALWAYS_INLINE void nsAtomicUtils::Xor(volatile nsInt32& dest, nsInt32 value)
{
  __sync_fetch_and_xor(&dest, value);
}

NS_ALWAYS_INLINE void nsAtomicUtils::Xor(volatile nsInt64& dest, nsInt64 value)
{
  __sync_fetch_and_xor_8(&dest, value);
}


NS_FORCE_INLINE void nsAtomicUtils::Min(volatile nsInt32& dest, nsInt32 value)
{
  // tries to exchange dest with the new value as long as the oldValue is not what we expected
  while (true)
  {
    nsInt32 iOldValue = dest;
    nsInt32 iNewValue = nsMath::Min(iOldValue, value);

    if (__sync_bool_compare_and_swap(&dest, iOldValue, iNewValue))
      break;
  }
}

NS_FORCE_INLINE void nsAtomicUtils::Min(volatile nsInt64& dest, nsInt64 value)
{
  // tries to exchange dest with the new value as long as the oldValue is not what we expected
  while (true)
  {
    nsInt64 iOldValue = dest;
    nsInt64 iNewValue = nsMath::Min(iOldValue, value);

    if (__sync_bool_compare_and_swap_8(&dest, iOldValue, iNewValue))
      break;
  }
}


NS_FORCE_INLINE void nsAtomicUtils::Max(volatile nsInt32& dest, nsInt32 value)
{
  // tries to exchange dest with the new value as long as the oldValue is not what we expected
  while (true)
  {
    nsInt32 iOldValue = dest;
    nsInt32 iNewValue = nsMath::Max(iOldValue, value);

    if (__sync_bool_compare_and_swap(&dest, iOldValue, iNewValue))
      break;
  }
}

NS_FORCE_INLINE void nsAtomicUtils::Max(volatile nsInt64& dest, nsInt64 value)
{
  // tries to exchange dest with the new value as long as the oldValue is not what we expected
  while (true)
  {
    nsInt64 iOldValue = dest;
    nsInt64 iNewValue = nsMath::Max(iOldValue, value);

    if (__sync_bool_compare_and_swap_8(&dest, iOldValue, iNewValue))
      break;
  }
}


NS_ALWAYS_INLINE nsInt32 nsAtomicUtils::Set(volatile nsInt32& dest, nsInt32 value)
{
  return __sync_lock_test_and_set(&dest, value);
}

NS_ALWAYS_INLINE nsInt64 nsAtomicUtils::Set(volatile nsInt64& dest, nsInt64 value)
{
  return __sync_lock_test_and_set_8(&dest, value);
}


NS_ALWAYS_INLINE bool nsAtomicUtils::TestAndSet(volatile nsInt32& dest, nsInt32 expected, nsInt32 value)
{
  return __sync_bool_compare_and_swap(&dest, expected, value);
}

NS_ALWAYS_INLINE bool nsAtomicUtils::TestAndSet(volatile nsInt64& dest, nsInt64 expected, nsInt64 value)
{
  return __sync_bool_compare_and_swap_8(&dest, expected, value);
}

NS_ALWAYS_INLINE bool nsAtomicUtils::TestAndSet(void** volatile dest, void* expected, void* value)
{
#if NS_ENABLED(NS_PLATFORM_64BIT)
  nsUInt64* puiTemp = reinterpret_cast<nsUInt64*>(dest);
  return __sync_bool_compare_and_swap(puiTemp, reinterpret_cast<nsUInt64>(expected), reinterpret_cast<nsUInt64>(value));
#else
  nsUInt32* puiTemp = reinterpret_cast<nsUInt32*>(dest);
  return __sync_bool_compare_and_swap(puiTemp, reinterpret_cast<nsUInt32>(expected), reinterpret_cast<nsUInt32>(value));
#endif
}

NS_ALWAYS_INLINE nsInt32 nsAtomicUtils::CompareAndSwap(volatile nsInt32& dest, nsInt32 expected, nsInt32 value)
{
  return __sync_val_compare_and_swap(&dest, expected, value);
}

NS_ALWAYS_INLINE nsInt64 nsAtomicUtils::CompareAndSwap(volatile nsInt64& dest, nsInt64 expected, nsInt64 value)
{
  return __sync_val_compare_and_swap_8(&dest, expected, value);
}
