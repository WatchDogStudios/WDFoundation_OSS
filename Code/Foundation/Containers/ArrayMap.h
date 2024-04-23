/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#pragma once

#include <Foundation/Containers/DynamicArray.h>

/// \brief An associative container, similar to nsMap, but all data is stored in a sorted contiguous array, which makes frequent lookups more
/// efficient.
///
/// Prefer this container over nsMap when you modify the container less often than you look things up (which is in most cases), and when
/// you do not need to store iterators to elements and require them to stay valid when the container is modified.
///
/// nsArrayMapBase also allows to store multiple values under the same key (like a multi-map).
template <typename KEY, typename VALUE>
class nsArrayMapBase
{
  /// \todo Custom comparer

public:
  struct Pair
  {
    KEY key;
    VALUE value;

    NS_DETECT_TYPE_CLASS(KEY, VALUE);

    NS_ALWAYS_INLINE bool operator<(const Pair& rhs) const { return key < rhs.key; }

    NS_ALWAYS_INLINE bool operator==(const Pair& rhs) const { return key == rhs.key; }
  };

  /// \brief Constructor.
  explicit nsArrayMapBase(nsAllocatorBase* pAllocator); // [tested]

  /// \brief Copy-Constructor.
  nsArrayMapBase(const nsArrayMapBase& rhs, nsAllocatorBase* pAllocator); // [tested]

  /// \brief Copy assignment operator.
  void operator=(const nsArrayMapBase& rhs); // [tested]

  /// \brief Returns the number of elements stored in the map.
  nsUInt32 GetCount() const; // [tested]

  /// \brief True if the map contains no elements.
  bool IsEmpty() const; // [tested]

  /// \brief Purges all elements from the map.
  void Clear(); // [tested]

  /// \brief Always inserts a new value under the given key. Duplicates are allowed.
  /// Returns the index of the newly added element.
  template <typename CompatibleKeyType, typename CompatibleValueType>
  nsUInt32 Insert(CompatibleKeyType&& key, CompatibleValueType&& value); // [tested]

  /// \brief Ensures the internal data structure is sorted. This is done automatically every time a lookup needs to be made.
  void Sort() const; // [tested]

  /// \brief Returns an index to one element with the given key. If the key is inserted multiple times, there is no guarantee which one is returned.
  /// Returns nsInvalidIndex when no such element exists.
  template <typename CompatibleKeyType>
  nsUInt32 Find(const CompatibleKeyType& key) const; // [tested]

  /// \brief Returns the index to the first element with a key equal or larger than the given key.
  /// Returns nsInvalidIndex when no such element exists.
  /// If there are multiple keys with the same value, the one at the smallest index is returned.
  template <typename CompatibleKeyType>
  nsUInt32 LowerBound(const CompatibleKeyType& key) const; // [tested]

  /// \brief Returns the index to the first element with a key that is LARGER than the given key.
  /// Returns nsInvalidIndex when no such element exists.
  /// If there are multiple keys with the same value, the one at the smallest index is returned.
  template <typename CompatibleKeyType>
  nsUInt32 UpperBound(const CompatibleKeyType& key) const; // [tested]

  /// \brief Returns the key that is stored at the given index.
  const KEY& GetKey(nsUInt32 uiIndex) const; // [tested]

  /// \brief Returns the value that is stored at the given index.
  const VALUE& GetValue(nsUInt32 uiIndex) const; // [tested]

  /// \brief Returns the value that is stored at the given index.
  VALUE& GetValue(nsUInt32 uiIndex); // [tested]

  /// \brief Returns a reference to the map data array.
  nsDynamicArray<Pair>& GetData();

  /// \brief Returns a constant reference to the map data array.
  const nsDynamicArray<Pair>& GetData() const;

  /// \brief Returns the value stored at the given key. If none exists, one is created. \a bExisted indicates whether an element needed to be created.
  template <typename CompatibleKeyType>
  VALUE& FindOrAdd(const CompatibleKeyType& key, bool* out_pExisted = nullptr); // [tested]

  /// \brief Same as FindOrAdd.
  template <typename CompatibleKeyType>
  VALUE& operator[](const CompatibleKeyType& key); // [tested]

  /// \brief Returns the key/value pair at the given index.
  const Pair& GetPair(nsUInt32 uiIndex) const; // [tested]

  /// \brief Removes the element at the given index.
  ///
  /// If the map is sorted and bKeepSorted is true, the element will be removed such that the map stays sorted.
  /// This is only useful, if only a single (or very few) elements are removed before the next lookup. If multiple values
  /// are removed, or new values are going to be inserted, as well, \a bKeepSorted should be left to false.
  void RemoveAtAndCopy(nsUInt32 uiIndex, bool bKeepSorted = false);

  /// \brief Removes one element with the given key. Returns true, if one was found and removed. If the same key exists multiple times, you need to
  /// call this function multiple times to remove them all.
  ///
  /// If the map is sorted and bKeepSorted is true, the element will be removed such that the map stays sorted.
  /// This is only useful, if only a single (or very few) elements are removed before the next lookup. If multiple values
  /// are removed, or new values are going to be inserted, as well, \a bKeepSorted should be left to false.
  template <typename CompatibleKeyType>
  bool RemoveAndCopy(const CompatibleKeyType& key, bool bKeepSorted = false); // [tested]

  /// \brief Returns whether an element with the given key exists.
  template <typename CompatibleKeyType>
  bool Contains(const CompatibleKeyType& key) const; // [tested]

  /// \brief Returns whether an element with the given key and value already exists.
  template <typename CompatibleKeyType>
  bool Contains(const CompatibleKeyType& key, const VALUE& value) const; // [tested]

  /// \brief Reserves enough memory to store \a size elements.
  void Reserve(nsUInt32 uiSize); // [tested]

  /// \brief Compacts the internal memory to not waste any space.
  void Compact(); // [tested]

  /// \brief Compares the two containers for equality.
  bool operator==(const nsArrayMapBase<KEY, VALUE>& rhs) const; // [tested]

  /// \brief Compares the two containers for equality.
  bool operator!=(const nsArrayMapBase<KEY, VALUE>& rhs) const; // [tested]

  /// \brief Returns the amount of bytes that are currently allocated on the heap.
  nsUInt64 GetHeapMemoryUsage() const { return m_Data.GetHeapMemoryUsage(); } // [tested]

  using const_iterator = typename nsDynamicArray<Pair>::const_iterator;
  using const_reverse_iterator = typename nsDynamicArray<Pair>::const_reverse_iterator;
  using iterator = typename nsDynamicArray<Pair>::iterator;
  using reverse_iterator = typename nsDynamicArray<Pair>::reverse_iterator;

private:
  mutable bool m_bSorted;
  mutable nsDynamicArray<Pair> m_Data;
};

/// \brief See nsArrayMapBase for details.
template <typename KEY, typename VALUE, typename AllocatorWrapper = nsDefaultAllocatorWrapper>
class nsArrayMap : public nsArrayMapBase<KEY, VALUE>
{
  NS_DECLARE_MEM_RELOCATABLE_TYPE();

public:
  nsArrayMap();
  explicit nsArrayMap(nsAllocatorBase* pAllocator);

  nsArrayMap(const nsArrayMap<KEY, VALUE, AllocatorWrapper>& rhs);
  nsArrayMap(const nsArrayMapBase<KEY, VALUE>& rhs);

  void operator=(const nsArrayMap<KEY, VALUE, AllocatorWrapper>& rhs);
  void operator=(const nsArrayMapBase<KEY, VALUE>& rhs);
};


template <typename KEY, typename VALUE>
typename nsArrayMapBase<KEY, VALUE>::iterator begin(nsArrayMapBase<KEY, VALUE>& ref_container)
{
  return begin(ref_container.GetData());
}

template <typename KEY, typename VALUE>
typename nsArrayMapBase<KEY, VALUE>::const_iterator begin(const nsArrayMapBase<KEY, VALUE>& container)
{
  return begin(container.GetData());
}
template <typename KEY, typename VALUE>
typename nsArrayMapBase<KEY, VALUE>::const_iterator cbegin(const nsArrayMapBase<KEY, VALUE>& container)
{
  return cbegin(container.GetData());
}

template <typename KEY, typename VALUE>
typename nsArrayMapBase<KEY, VALUE>::reverse_iterator rbegin(nsArrayMapBase<KEY, VALUE>& ref_container)
{
  return rbegin(ref_container.GetData());
}

template <typename KEY, typename VALUE>
typename nsArrayMapBase<KEY, VALUE>::const_reverse_iterator rbegin(const nsArrayMapBase<KEY, VALUE>& container)
{
  return rbegin(container.GetData());
}

template <typename KEY, typename VALUE>
typename nsArrayMapBase<KEY, VALUE>::const_reverse_iterator crbegin(const nsArrayMapBase<KEY, VALUE>& container)
{
  return crbegin(container.GetData());
}

template <typename KEY, typename VALUE>
typename nsArrayMapBase<KEY, VALUE>::iterator end(nsArrayMapBase<KEY, VALUE>& ref_container)
{
  return end(ref_container.GetData());
}

template <typename KEY, typename VALUE>
typename nsArrayMapBase<KEY, VALUE>::const_iterator end(const nsArrayMapBase<KEY, VALUE>& container)
{
  return end(container.GetData());
}

template <typename KEY, typename VALUE>
typename nsArrayMapBase<KEY, VALUE>::const_iterator cend(const nsArrayMapBase<KEY, VALUE>& container)
{
  return cend(container.GetData());
}

template <typename KEY, typename VALUE>
typename nsArrayMapBase<KEY, VALUE>::reverse_iterator rend(nsArrayMapBase<KEY, VALUE>& ref_container)
{
  return rend(ref_container.GetData());
}

template <typename KEY, typename VALUE>
typename nsArrayMapBase<KEY, VALUE>::const_reverse_iterator rend(const nsArrayMapBase<KEY, VALUE>& container)
{
  return rend(container.GetData());
}

template <typename KEY, typename VALUE>
typename nsArrayMapBase<KEY, VALUE>::const_reverse_iterator crend(const nsArrayMapBase<KEY, VALUE>& container)
{
  return crend(container.GetData());
}


#include <Foundation/Containers/Implementation/ArrayMap_inl.h>
