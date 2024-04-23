/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#ifdef NS_ATOMICUTLS_WIN_INL_H_INCLUDED
#  error "This file must not be included twice."
#endif

#define NS_ATOMICUTLS_WIN_INL_H_INCLUDED

#include <intrin.h>

NS_ALWAYS_INLINE nsInt32 nsAtomicUtils::Read(volatile const nsInt32& iSrc)
{
  return _InterlockedOr((volatile long*)(&iSrc), 0);
}

NS_ALWAYS_INLINE nsInt64 nsAtomicUtils::Read(volatile const nsInt64& iSrc)
{
#if NS_ENABLED(NS_PLATFORM_32BIT)
  nsInt64 old;
  do
  {
    old = iSrc;
  } while (_InterlockedCompareExchange64(const_cast<volatile nsInt64*>(&iSrc), old, old) != old);
  return old;
#else
  return _InterlockedOr64(const_cast<volatile nsInt64*>(&iSrc), 0);
#endif
}

NS_ALWAYS_INLINE nsInt32 nsAtomicUtils::Increment(volatile nsInt32& ref_iDest)
{
  return _InterlockedIncrement(reinterpret_cast<volatile long*>(&ref_iDest));
}

NS_ALWAYS_INLINE nsInt64 nsAtomicUtils::Increment(volatile nsInt64& ref_iDest)
{
#if NS_ENABLED(NS_PLATFORM_32BIT)
  nsInt64 old;
  do
  {
    old = ref_iDest;
  } while (_InterlockedCompareExchange64(&ref_iDest, old + 1, old) != old);
  return old + 1;
#else
  return _InterlockedIncrement64(&ref_iDest);
#endif
}

NS_ALWAYS_INLINE nsInt32 nsAtomicUtils::Decrement(volatile nsInt32& ref_iDest)
{
  return _InterlockedDecrement(reinterpret_cast<volatile long*>(&ref_iDest));
}

NS_ALWAYS_INLINE nsInt64 nsAtomicUtils::Decrement(volatile nsInt64& ref_iDest)
{
#if NS_ENABLED(NS_PLATFORM_32BIT)
  nsInt64 old;
  do
  {
    old = ref_iDest;
  } while (_InterlockedCompareExchange64(&ref_iDest, old - 1, old) != old);
  return old - 1;
#else
  return _InterlockedDecrement64(&ref_iDest);
#endif
}

NS_ALWAYS_INLINE nsInt32 nsAtomicUtils::PostIncrement(volatile nsInt32& ref_iDest)
{
  return _InterlockedExchangeAdd(reinterpret_cast<volatile long*>(&ref_iDest), 1);
}

NS_ALWAYS_INLINE nsInt64 nsAtomicUtils::PostIncrement(volatile nsInt64& ref_iDest)
{
#if NS_ENABLED(NS_PLATFORM_32BIT)
  nsInt64 old;
  do
  {
    old = ref_iDest;
  } while (_InterlockedCompareExchange64(&ref_iDest, old + 1, old) != old);
  return old;
#else
  return _InterlockedExchangeAdd64(&ref_iDest, 1);
#endif
}

NS_ALWAYS_INLINE nsInt32 nsAtomicUtils::PostDecrement(volatile nsInt32& ref_iDest)
{
  return _InterlockedExchangeAdd(reinterpret_cast<volatile long*>(&ref_iDest), -1);
}

NS_ALWAYS_INLINE nsInt64 nsAtomicUtils::PostDecrement(volatile nsInt64& ref_iDest)
{
#if NS_ENABLED(NS_PLATFORM_32BIT)
  nsInt64 old;
  do
  {
    old = ref_iDest;
  } while (_InterlockedCompareExchange64(&ref_iDest, old - 1, old) != old);
  return old;
#else
  return _InterlockedExchangeAdd64(&ref_iDest, -1);
#endif
}

NS_ALWAYS_INLINE void nsAtomicUtils::Add(volatile nsInt32& ref_iDest, nsInt32 value)
{
  _InterlockedExchangeAdd(reinterpret_cast<volatile long*>(&ref_iDest), value);
}

NS_ALWAYS_INLINE void nsAtomicUtils::Add(volatile nsInt64& ref_iDest, nsInt64 value)
{
#if NS_ENABLED(NS_PLATFORM_32BIT)
  nsInt64 old;
  do
  {
    old = ref_iDest;
  } while (_InterlockedCompareExchange64(&ref_iDest, old + value, old) != old);
#else
  _InterlockedExchangeAdd64(&ref_iDest, value);
#endif
}


NS_ALWAYS_INLINE void nsAtomicUtils::And(volatile nsInt32& ref_iDest, nsInt32 value)
{
  _InterlockedAnd(reinterpret_cast<volatile long*>(&ref_iDest), value);
}

NS_ALWAYS_INLINE void nsAtomicUtils::And(volatile nsInt64& ref_iDest, nsInt64 value)
{
#if NS_ENABLED(NS_PLATFORM_32BIT)
  nsInt64 old;
  do
  {
    old = ref_iDest;
  } while (_InterlockedCompareExchange64(&ref_iDest, old & value, old) != old);
#else
  _InterlockedAnd64(&ref_iDest, value);
#endif
}


NS_ALWAYS_INLINE void nsAtomicUtils::Or(volatile nsInt32& ref_iDest, nsInt32 value)
{
  _InterlockedOr(reinterpret_cast<volatile long*>(&ref_iDest), value);
}

NS_ALWAYS_INLINE void nsAtomicUtils::Or(volatile nsInt64& ref_iDest, nsInt64 value)
{
#if NS_ENABLED(NS_PLATFORM_32BIT)
  nsInt64 old;
  do
  {
    old = ref_iDest;
  } while (_InterlockedCompareExchange64(&ref_iDest, old | value, old) != old);
#else
  _InterlockedOr64(&ref_iDest, value);
#endif
}


NS_ALWAYS_INLINE void nsAtomicUtils::Xor(volatile nsInt32& ref_iDest, nsInt32 value)
{
  _InterlockedXor(reinterpret_cast<volatile long*>(&ref_iDest), value);
}

NS_ALWAYS_INLINE void nsAtomicUtils::Xor(volatile nsInt64& ref_iDest, nsInt64 value)
{
#if NS_ENABLED(NS_PLATFORM_32BIT)
  nsInt64 old;
  do
  {
    old = ref_iDest;
  } while (_InterlockedCompareExchange64(&ref_iDest, old ^ value, old) != old);
#else
  _InterlockedXor64(&ref_iDest, value);
#endif
}


inline void nsAtomicUtils::Min(volatile nsInt32& ref_iDest, nsInt32 value)
{
  // tries to exchange dest with the new value as long as the oldValue is not what we expected
  while (true)
  {
    nsInt32 iOldValue = ref_iDest;
    nsInt32 iNewValue = value < iOldValue ? value : iOldValue; // do Min manually here, to break #include cycles

    if (_InterlockedCompareExchange(reinterpret_cast<volatile long*>(&ref_iDest), iNewValue, iOldValue) == iOldValue)
      break;
  }
}

inline void nsAtomicUtils::Min(volatile nsInt64& ref_iDest, nsInt64 value)
{
  // tries to exchange dest with the new value as long as the oldValue is not what we expected
  while (true)
  {
    nsInt64 iOldValue = ref_iDest;
    nsInt64 iNewValue = value < iOldValue ? value : iOldValue; // do Min manually here, to break #include cycles

    if (_InterlockedCompareExchange64(&ref_iDest, iNewValue, iOldValue) == iOldValue)
      break;
  }
}

inline void nsAtomicUtils::Max(volatile nsInt32& ref_iDest, nsInt32 value)
{
  // tries to exchange dest with the new value as long as the oldValue is not what we expected
  while (true)
  {
    nsInt32 iOldValue = ref_iDest;
    nsInt32 iNewValue = iOldValue < value ? value : iOldValue; // do Max manually here, to break #include cycles

    if (_InterlockedCompareExchange(reinterpret_cast<volatile long*>(&ref_iDest), iNewValue, iOldValue) == iOldValue)
      break;
  }
}

inline void nsAtomicUtils::Max(volatile nsInt64& ref_iDest, nsInt64 value)
{
  // tries to exchange dest with the new value as long as the oldValue is not what we expected
  while (true)
  {
    nsInt64 iOldValue = ref_iDest;
    nsInt64 iNewValue = iOldValue < value ? value : iOldValue; // do Max manually here, to break #include cycles

    if (_InterlockedCompareExchange64(&ref_iDest, iNewValue, iOldValue) == iOldValue)
      break;
  }
}


inline nsInt32 nsAtomicUtils::Set(volatile nsInt32& ref_iDest, nsInt32 value)
{
  return _InterlockedExchange(reinterpret_cast<volatile long*>(&ref_iDest), value);
}

NS_ALWAYS_INLINE nsInt64 nsAtomicUtils::Set(volatile nsInt64& ref_iDest, nsInt64 value)
{
#if NS_ENABLED(NS_PLATFORM_32BIT)
  nsInt64 old;
  do
  {
    old = ref_iDest;
  } while (_InterlockedCompareExchange64(&ref_iDest, value, old) != old);
  return old;
#else
  return _InterlockedExchange64(&ref_iDest, value);
#endif
}


NS_ALWAYS_INLINE bool nsAtomicUtils::TestAndSet(volatile nsInt32& ref_iDest, nsInt32 iExpected, nsInt32 value)
{
  return _InterlockedCompareExchange(reinterpret_cast<volatile long*>(&ref_iDest), value, iExpected) == iExpected;
}

NS_ALWAYS_INLINE bool nsAtomicUtils::TestAndSet(volatile nsInt64& ref_iDest, nsInt64 iExpected, nsInt64 value)
{
  return _InterlockedCompareExchange64(&ref_iDest, value, iExpected) == iExpected;
}

NS_ALWAYS_INLINE bool nsAtomicUtils::TestAndSet(void** volatile pDest, void* pExpected, void* value)
{
  return _InterlockedCompareExchangePointer(pDest, value, pExpected) == pExpected;
}

NS_ALWAYS_INLINE nsInt32 nsAtomicUtils::CompareAndSwap(volatile nsInt32& ref_iDest, nsInt32 iExpected, nsInt32 value)
{
  return _InterlockedCompareExchange(reinterpret_cast<volatile long*>(&ref_iDest), value, iExpected);
}

NS_ALWAYS_INLINE nsInt64 nsAtomicUtils::CompareAndSwap(volatile nsInt64& ref_iDest, nsInt64 iExpected, nsInt64 value)
{
  return _InterlockedCompareExchange64(&ref_iDest, value, iExpected);
}
