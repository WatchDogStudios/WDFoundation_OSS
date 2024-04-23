/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */

#define NS_CHECK_CLASS(T)                                 \
  NS_CHECK_AT_COMPILETIME_MSG(!std::is_trivial<T>::value, \
    "POD type is treated as class. Use NS_DECLARE_POD_TYPE(YourClass) or NS_DEFINE_AS_POD_TYPE(ExternalClass) to mark it as POD.")

// public methods: redirect to implementation
template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Construct(T* pDestination, size_t uiCount)
{
  // Default constructor is always called, so that debug helper initializations (e.g. nsVec3 initializes to NaN) take place.
  // Note that destructor is ONLY called for class types.
  // Special case for c++11 to prevent default construction of "real" Pod types, also avoids warnings on msvc
  Construct(pDestination, uiCount, nsTraitInt < nsIsPodType<T>::value && std::is_trivial<T>::value > ());
}

template <typename T>
NS_ALWAYS_INLINE nsMemoryUtils::ConstructorFunction nsMemoryUtils::MakeConstructorFunction()
{
  return MakeConstructorFunction<T>(nsTraitInt < nsIsPodType<T>::value && std::is_trivial<T>::value > ());
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::DefaultConstruct(T* pDestination, size_t uiCount)
{
  for (size_t i = 0; i < uiCount; i++)
  {
    ::new (pDestination + i) T();
  }
}

template <typename T>
NS_ALWAYS_INLINE nsMemoryUtils::ConstructorFunction nsMemoryUtils::MakeDefaultConstructorFunction()
{
  struct Helper
  {
    static void DefaultConstruct(void* pDestination) { nsMemoryUtils::DefaultConstruct(static_cast<T*>(pDestination), 1); }
  };

  return &Helper::DefaultConstruct;
}

template <typename Destination, typename Source>
NS_ALWAYS_INLINE void nsMemoryUtils::CopyConstruct(Destination* pDestination, const Source& copy, size_t uiCount)
{
  CopyConstruct<Destination, Source>(pDestination, copy, uiCount, nsIsPodType<Destination>());
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::CopyConstructArray(T* pDestination, const T* pSource, size_t uiCount)
{
  NS_ASSERT_DEV(pDestination < pSource || pSource + uiCount <= pDestination, "Memory regions must not overlap when using CopyConstruct.");
  CopyConstructArray<T>(pDestination, pSource, uiCount, nsIsPodType<T>());
}

template <typename T>
NS_ALWAYS_INLINE nsMemoryUtils::CopyConstructorFunction nsMemoryUtils::MakeCopyConstructorFunction()
{
  struct Helper
  {
    static void CopyConstruct(void* pDestination, const void* pSource)
    {
      nsMemoryUtils::CopyConstruct(static_cast<T*>(pDestination), *static_cast<const T*>(pSource), 1);
    }
  };

  return &Helper::CopyConstruct;
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::MoveConstruct(T* pDestination, T&& source)
{
  // Make sure source is actually an rvalue reference (T&& is a universal reference).
  static_assert(std::is_rvalue_reference<decltype(source)>::value, "'source' parameter is not an rvalue reference.");
  ::new (pDestination) T(std::forward<T>(source));
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::MoveConstruct(T* pDestination, T* pSource, size_t uiCount)
{
  NS_ASSERT_DEV(pDestination < pSource || pSource + uiCount <= pDestination, "Memory regions must not overlap when using MoveConstruct.");

  // Enforce move construction.
  static_assert(std::is_move_constructible<T>::value, "Type is not move constructible!");

  for (size_t i = 0; i < uiCount; ++i)
  {
    ::new (pDestination + i) T(std::move(pSource[i]));
  }
}

template <typename Destination, typename Source>
NS_ALWAYS_INLINE void nsMemoryUtils::CopyOrMoveConstruct(Destination* pDestination, Source&& source)
{
  using IsRValueRef = typename std::is_rvalue_reference<decltype(source)>::type;
  CopyOrMoveConstruct<Destination, Source>(pDestination, std::forward<Source>(source), IsRValueRef());
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::RelocateConstruct(T* pDestination, T* pSource, size_t uiCount)
{
  NS_ASSERT_DEV(pDestination < pSource || pSource + uiCount <= pDestination, "Memory regions must not overlap when using RelocateConstruct.");
  RelocateConstruct(pDestination, pSource, uiCount, nsGetTypeClass<T>());
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Destruct(T* pDestination, size_t uiCount)
{
  Destruct(pDestination, uiCount, nsIsPodType<T>());
}

template <typename T>
NS_ALWAYS_INLINE nsMemoryUtils::DestructorFunction nsMemoryUtils::MakeDestructorFunction()
{
  return MakeDestructorFunction<T>(nsIsPodType<T>());
}

NS_ALWAYS_INLINE void nsMemoryUtils::RawByteCopy(void* pDestination, const void* pSource, size_t uiNumBytesToCopy)
{
  memcpy(pDestination, pSource, uiNumBytesToCopy);
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Copy(T* pDestination, const T* pSource, size_t uiCount)
{
  NS_ASSERT_DEV(
    pDestination < pSource || pSource + uiCount <= pDestination, "Memory regions must not overlap when using Copy. Use CopyOverlapped instead.");
  Copy(pDestination, pSource, uiCount, nsIsPodType<T>());
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::CopyOverlapped(T* pDestination, const T* pSource, size_t uiCount)
{
  CopyOverlapped(pDestination, pSource, uiCount, nsIsPodType<T>());
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Relocate(T* pDestination, T* pSource, size_t uiCount)
{
  NS_ASSERT_DEV(pDestination < pSource || pSource + uiCount <= pDestination, "Memory regions must not overlap when using Relocate.");
  Relocate(pDestination, pSource, uiCount, nsGetTypeClass<T>());
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::RelocateOverlapped(T* pDestination, T* pSource, size_t uiCount)
{
  RelocateOverlapped(pDestination, pSource, uiCount, nsGetTypeClass<T>());
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Prepend(T* pDestination, const T& source, size_t uiCount)
{
  Prepend(pDestination, source, uiCount, nsGetTypeClass<T>());
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Prepend(T* pDestination, T&& source, size_t uiCount)
{
  Prepend(pDestination, std::move(source), uiCount, nsGetTypeClass<T>());
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Prepend(T* pDestination, const T* pSource, size_t uiSourceCount, size_t uiCount)
{
  Prepend(pDestination, pSource, uiSourceCount, uiCount, nsGetTypeClass<T>());
}

template <typename T>
NS_ALWAYS_INLINE bool nsMemoryUtils::IsEqual(const T* a, const T* b, size_t uiCount /*= 1*/)
{
  return IsEqual(a, b, uiCount, nsIsPodType<T>());
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::ZeroFill(T* pDestination, size_t uiCount)
{
  memset(pDestination, 0, uiCount * sizeof(T));
}

template <typename T, size_t N>
NS_ALWAYS_INLINE void nsMemoryUtils::ZeroFillArray(T (&destination)[N])
{
  return ZeroFill(destination, N);
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::PatternFill(T* pDestination, nsUInt8 uiBytePattern, size_t uiCount)
{
  memset(pDestination, uiBytePattern, uiCount * sizeof(T));
}

template <typename T, size_t N>
NS_ALWAYS_INLINE void nsMemoryUtils::PatternFillArray(T (&destination)[N], nsUInt8 uiBytePattern)
{
  return PatternFill(destination, uiBytePattern, N);
}

template <typename T>
NS_ALWAYS_INLINE nsInt32 nsMemoryUtils::Compare(const T* a, const T* b, size_t uiCount /*= 1*/)
{
  return memcmp(a, b, uiCount * sizeof(T));
}

NS_ALWAYS_INLINE nsInt32 nsMemoryUtils::RawByteCompare(const void* a, const void* b, size_t uiNumBytesToCompare)
{
  return memcmp(a, b, uiNumBytesToCompare);
}

template <typename T>
NS_ALWAYS_INLINE T* nsMemoryUtils::AddByteOffset(T* pPtr, ptrdiff_t iOffset)
{
  return reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(pPtr) + iOffset);
}

template <typename T>
NS_ALWAYS_INLINE T* nsMemoryUtils::AlignBackwards(T* pPtr, size_t uiAlignment)
{
  return reinterpret_cast<T*>(reinterpret_cast<size_t>(pPtr) & ~(uiAlignment - 1));
}

template <typename T>
NS_ALWAYS_INLINE T* nsMemoryUtils::AlignForwards(T* pPtr, size_t uiAlignment)
{
  return reinterpret_cast<T*>((reinterpret_cast<size_t>(pPtr) + uiAlignment - 1) & ~(uiAlignment - 1));
}

template <typename T>
NS_ALWAYS_INLINE T nsMemoryUtils::AlignSize(T uiSize, T uiAlignment)
{
  return ((uiSize + (uiAlignment - 1)) & ~(uiAlignment - 1));
}

template <typename T>
NS_ALWAYS_INLINE bool nsMemoryUtils::IsAligned(const T* pPtr, size_t uiAlignment)
{
  return (reinterpret_cast<size_t>(pPtr) & (uiAlignment - 1)) == 0;
}

template <typename T>
NS_ALWAYS_INLINE bool nsMemoryUtils::IsSizeAligned(T uiSize, T uiAlignment)
{
  return (uiSize & (uiAlignment - 1)) == 0;
}

// private methods

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Construct(T* pDestination, size_t uiCount, nsTypeIsPod)
{
  NS_CHECK_AT_COMPILETIME_MSG(std::is_trivial<T>::value, "This method should only be called for 'real' pod aka trivial types");
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Construct(T* pDestination, size_t uiCount, nsTypeIsClass)
{
  NS_CHECK_CLASS(T);

#define NS_GCC_WARNING_NAME "-Wstringop-overflow"
#include <Foundation/Basics/Compiler/GCC/DisableWarning_GCC.h>

  for (size_t i = 0; i < uiCount; i++)
  {
    ::new (pDestination + i) T();
  }

#include <Foundation/Basics/Compiler/GCC/RestoreWarning_GCC.h>
}

template <typename T>
NS_ALWAYS_INLINE nsMemoryUtils::ConstructorFunction nsMemoryUtils::MakeConstructorFunction(nsTypeIsPod)
{
  NS_CHECK_AT_COMPILETIME_MSG(std::is_trivial<T>::value, "This method should only be called for 'real' pod aka trivial types");
  return nullptr;
}

template <typename T>
NS_ALWAYS_INLINE nsMemoryUtils::ConstructorFunction nsMemoryUtils::MakeConstructorFunction(nsTypeIsClass)
{
  NS_CHECK_CLASS(T);

  struct Helper
  {
    static void Construct(void* pDestination) { nsMemoryUtils::Construct(static_cast<T*>(pDestination), 1, nsTypeIsClass()); }
  };

  return &Helper::Construct;
}

template <typename Destination, typename Source>
NS_ALWAYS_INLINE void nsMemoryUtils::CopyConstruct(Destination* pDestination, const Source& copy, size_t uiCount, nsTypeIsPod)
{
  static_assert(std::is_same<Destination, Source>::value ||
                  (std::is_base_of<Destination, Source>::value == false && std::is_base_of<Source, Destination>::value == false),
    "Can't copy POD types that are derived from each other. Are you certain any of these types should be POD?");

  const Destination& copyConverted = copy;
  for (size_t i = 0; i < uiCount; i++)
  {
    memcpy(pDestination + i, &copyConverted, sizeof(Destination));
  }
}

template <typename Destination, typename Source>
NS_ALWAYS_INLINE void nsMemoryUtils::CopyConstruct(Destination* pDestination, const Source& copy, size_t uiCount, nsTypeIsClass)
{
  NS_CHECK_CLASS(Destination);

  for (size_t i = 0; i < uiCount; i++)
  {
    ::new (pDestination + i) Destination(copy); // Note that until now copy has not been converted to Destination. This allows for calling
                                                // specialized constructors if available.
  }
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::CopyConstructArray(T* pDestination, const T* pSource, size_t uiCount, nsTypeIsPod)
{
  memcpy(pDestination, pSource, uiCount * sizeof(T));
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::CopyConstructArray(T* pDestination, const T* pSource, size_t uiCount, nsTypeIsClass)
{
  NS_CHECK_CLASS(T);

  for (size_t i = 0; i < uiCount; i++)
  {
    ::new (pDestination + i) T(pSource[i]);
  }
}

template <typename Destination, typename Source>
NS_ALWAYS_INLINE void nsMemoryUtils::CopyOrMoveConstruct(Destination* pDestination, const Source& source, NotRValueReference)
{
  CopyConstruct<Destination, Source>(pDestination, source, 1);
}

template <typename Destination, typename Source>
NS_ALWAYS_INLINE void nsMemoryUtils::CopyOrMoveConstruct(Destination* pDestination, Source&& source, IsRValueReference)
{
  static_assert(std::is_rvalue_reference<decltype(source)>::value,
    "Implementation Error: This version of CopyOrMoveConstruct should only be called with a rvalue reference!");
  ::new (pDestination) Destination(std::move(source));
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::RelocateConstruct(T* pDestination, T* pSource, size_t uiCount, nsTypeIsPod)
{
  memcpy(pDestination, pSource, uiCount * sizeof(T));
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::RelocateConstruct(T* pDestination, T* pSource, size_t uiCount, nsTypeIsMemRelocatable)
{
  memcpy(pDestination, pSource, uiCount * sizeof(T));
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::RelocateConstruct(T* pDestination, T* pSource, size_t uiCount, nsTypeIsClass)
{
  NS_CHECK_CLASS(T);

  for (size_t i = 0; i < uiCount; i++)
  {
    // Note that this calls the move constructor only if available and will copy otherwise.
    ::new (pDestination + i) T(std::move(pSource[i]));
  }

  Destruct(pSource, uiCount, nsTypeIsClass());
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Destruct(T* pDestination, size_t uiCount, nsTypeIsPod)
{
  // Nothing to do here. See Construct of for more info.

  static_assert(std::is_trivially_destructible<T>::value != 0, "Class is declared as POD but has a non-trivial destructor. Remove the destructor or don't declare it as POD.");
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Destruct(T* pDestination, size_t uiCount, nsTypeIsClass)
{
  NS_CHECK_CLASS(T);

#define NS_GCC_WARNING_NAME "-Waggressive-loop-optimizations"
#include <Foundation/Basics/Compiler/GCC/DisableWarning_GCC.h>

  for (size_t i = uiCount; i > 0; --i)
  {
    pDestination[i - 1].~T();
  }

#include <Foundation/Basics/Compiler/GCC/RestoreWarning_GCC.h>
}

template <typename T>
NS_ALWAYS_INLINE nsMemoryUtils::DestructorFunction nsMemoryUtils::MakeDestructorFunction(nsTypeIsPod)
{
  return nullptr;
}

template <typename T>
NS_ALWAYS_INLINE nsMemoryUtils::DestructorFunction nsMemoryUtils::MakeDestructorFunction(nsTypeIsClass)
{
  NS_CHECK_CLASS(T);

  struct Helper
  {
    static void Destruct(void* pDestination) { nsMemoryUtils::Destruct(static_cast<T*>(pDestination), 1, nsTypeIsClass()); }
  };

  return &Helper::Destruct;
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Copy(T* pDestination, const T* pSource, size_t uiCount, nsTypeIsPod)
{
  memcpy(pDestination, pSource, uiCount * sizeof(T));
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Copy(T* pDestination, const T* pSource, size_t uiCount, nsTypeIsClass)
{
  NS_CHECK_CLASS(T);

  for (size_t i = 0; i < uiCount; i++)
  {
    pDestination[i] = pSource[i];
  }
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::CopyOverlapped(T* pDestination, const T* pSource, size_t uiCount, nsTypeIsPod)
{
  memmove(pDestination, pSource, uiCount * sizeof(T));
}

template <typename T>
inline void nsMemoryUtils::CopyOverlapped(T* pDestination, const T* pSource, size_t uiCount, nsTypeIsClass)
{
  NS_CHECK_CLASS(T);

  if (pDestination == pSource)
    return;

  if (pDestination < pSource)
  {
    for (size_t i = 0; i < uiCount; i++)
    {
      pDestination[i] = pSource[i];
    }
  }
  else
  {
    for (size_t i = uiCount; i > 0; --i)
    {
      pDestination[i - 1] = pSource[i - 1];
    }
  }
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Relocate(T* pDestination, T* pSource, size_t uiCount, nsTypeIsPod)
{
  memcpy(pDestination, pSource, uiCount * sizeof(T));
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Relocate(T* pDestination, T* pSource, size_t uiCount, nsTypeIsMemRelocatable)
{
  memcpy(pDestination, pSource, uiCount * sizeof(T));
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Relocate(T* pDestination, T* pSource, size_t uiCount, nsTypeIsClass)
{
  NS_CHECK_CLASS(T);

  for (size_t i = 0; i < uiCount; i++)
  {
    // Note that this calls the move constructor only if available and will copy otherwise.
    pDestination[i] = std::move(pSource[i]);
  }

  Destruct(pSource, uiCount, nsTypeIsClass());
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::RelocateOverlapped(T* pDestination, T* pSource, size_t uiCount, nsTypeIsPod)
{
  memmove(pDestination, pSource, uiCount * sizeof(T));
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::RelocateOverlapped(T* pDestination, T* pSource, size_t uiCount, nsTypeIsMemRelocatable)
{
  if (pDestination < pSource)
  {
    size_t uiDestructCount = pSource - pDestination;
    Destruct(pDestination, uiDestructCount, nsTypeIsClass());
  }
  else
  {
    size_t uiDestructCount = pDestination - pSource;
    Destruct(pSource + uiCount, uiDestructCount, nsTypeIsClass());
  }
  memmove(pDestination, pSource, uiCount * sizeof(T));
}

template <typename T>
inline void nsMemoryUtils::RelocateOverlapped(T* pDestination, T* pSource, size_t uiCount, nsTypeIsClass)
{
  NS_CHECK_CLASS(T);

  if (pDestination == pSource)
    return;

  if (pDestination < pSource)
  {
    for (size_t i = 0; i < uiCount; i++)
    {
      pDestination[i] = std::move(pSource[i]);
    }

    size_t uiDestructCount = pSource - pDestination;
    Destruct(pSource + uiCount - uiDestructCount, uiDestructCount, nsTypeIsClass());
  }
  else
  {
    for (size_t i = uiCount; i > 0; --i)
    {
      pDestination[i - 1] = std::move(pSource[i - 1]);
    }

    size_t uiDestructCount = pDestination - pSource;
    Destruct(pSource, uiDestructCount, nsTypeIsClass());
  }
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Prepend(T* pDestination, const T& source, size_t uiCount, nsTypeIsPod)
{
  memmove(pDestination + 1, pDestination, uiCount * sizeof(T));
  CopyConstruct(pDestination, source, 1, nsTypeIsPod());
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Prepend(T* pDestination, const T& source, size_t uiCount, nsTypeIsMemRelocatable)
{
  memmove(pDestination + 1, pDestination, uiCount * sizeof(T));
  CopyConstruct(pDestination, source, 1, nsTypeIsClass());
}

template <typename T>
inline void nsMemoryUtils::Prepend(T* pDestination, const T& source, size_t uiCount, nsTypeIsClass)
{
  NS_CHECK_CLASS(T);

  if (uiCount > 0)
  {
    MoveConstruct(pDestination + uiCount, std::move(pDestination[uiCount - 1]));

    for (size_t i = uiCount - 1; i > 0; --i)
    {
      pDestination[i] = std::move(pDestination[i - 1]);
    }

    *pDestination = source;
  }
  else
  {
    CopyConstruct(pDestination, source, 1, nsTypeIsClass());
  }
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Prepend(T* pDestination, T&& source, size_t uiCount, nsTypeIsPod)
{
  memmove(pDestination + 1, pDestination, uiCount * sizeof(T));
  MoveConstruct(pDestination, std::move(source));
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Prepend(T* pDestination, T&& source, size_t uiCount, nsTypeIsMemRelocatable)
{
  memmove(pDestination + 1, pDestination, uiCount * sizeof(T));
  MoveConstruct(pDestination, std::move(source));
}

template <typename T>
inline void nsMemoryUtils::Prepend(T* pDestination, T&& source, size_t uiCount, nsTypeIsClass)
{
  NS_CHECK_CLASS(T);

  if (uiCount > 0)
  {
    MoveConstruct(pDestination + uiCount, std::move(pDestination[uiCount - 1]));

    for (size_t i = uiCount - 1; i > 0; --i)
    {
      pDestination[i] = std::move(pDestination[i - 1]);
    }

    *pDestination = std::move(source);
  }
  else
  {
    MoveConstruct(pDestination, std::move(source));
  }
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Prepend(T* pDestination, const T* pSource, size_t uiSourceCount, size_t uiCount, nsTypeIsPod)
{
  memmove(pDestination + uiSourceCount, pDestination, uiCount * sizeof(T));
  CopyConstructArray(pDestination, pSource, uiSourceCount, nsTypeIsPod());
}

template <typename T>
NS_ALWAYS_INLINE void nsMemoryUtils::Prepend(T* pDestination, const T* pSource, size_t uiSourceCount, size_t uiCount, nsTypeIsMemRelocatable)
{
  memmove(pDestination + uiSourceCount, pDestination, uiCount * sizeof(T));
  CopyConstructArray(pDestination, pSource, uiSourceCount, nsTypeIsClass());
}

template <typename T>
inline void nsMemoryUtils::Prepend(T* pDestination, const T* pSource, size_t uiSourceCount, size_t uiCount, nsTypeIsClass)
{
  NS_CHECK_CLASS(T);

  if (uiCount > 0)
  {
    MoveConstruct(pDestination + uiSourceCount, pDestination, uiCount);
    CopyConstructArray(pDestination, pSource, uiSourceCount, nsTypeIsClass());
  }
  else
  {
    CopyConstructArray(pDestination, pSource, uiSourceCount, nsTypeIsClass());
  }
}

template <typename T>
NS_ALWAYS_INLINE bool nsMemoryUtils::IsEqual(const T* a, const T* b, size_t uiCount, nsTypeIsPod)
{
  return memcmp(a, b, uiCount * sizeof(T)) == 0;
}

template <typename T>
NS_ALWAYS_INLINE bool nsMemoryUtils::IsEqual(const T* a, const T* b, size_t uiCount, nsTypeIsClass)
{
  NS_CHECK_CLASS(T);

  for (size_t i = 0; i < uiCount; i++)
  {
    if (!(a[i] == b[i]))
      return false;
  }
  return true;
}


#undef NS_CHECK_CLASS
