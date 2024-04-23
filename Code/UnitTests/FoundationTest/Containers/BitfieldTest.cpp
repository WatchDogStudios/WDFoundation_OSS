/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <FoundationTest/FoundationTestPCH.h>

#include <Foundation/Containers/Bitfield.h>
#include <Foundation/Containers/Deque.h>
#include <Foundation/Strings/String.h>

NS_CREATE_SIMPLE_TEST(Containers, Bitfield)
{
  NS_TEST_BLOCK(nsTestBlock::Enabled, "GetCount / IsEmpty / Clear")
  {
    nsDynamicBitfield bf; // using a dynamic array

    NS_TEST_INT(bf.GetCount(), 0);
    NS_TEST_BOOL(bf.IsEmpty());

    bf.SetCount(15, false);

    NS_TEST_INT(bf.GetCount(), 15);
    NS_TEST_BOOL(!bf.IsEmpty());

    bf.Clear();

    NS_TEST_INT(bf.GetCount(), 0);
    NS_TEST_BOOL(bf.IsEmpty());

    bf.SetCount(37, false);

    NS_TEST_INT(bf.GetCount(), 37);
    NS_TEST_BOOL(!bf.IsEmpty());
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "SetCount / SetAllBits / ClearAllBits")
  {
    nsHybridBitfield<512> bf; // using a hybrid array

    bf.SetCount(249, false);
    NS_TEST_INT(bf.GetCount(), 249);

    for (nsUInt32 i = 0; i < bf.GetCount(); ++i)
      NS_TEST_BOOL(!bf.IsBitSet(i));

    bf.SetAllBits();
    NS_TEST_INT(bf.GetCount(), 249);

    for (nsUInt32 i = 0; i < bf.GetCount(); ++i)
      NS_TEST_BOOL(bf.IsBitSet(i));

    bf.ClearAllBits();
    NS_TEST_INT(bf.GetCount(), 249);

    for (nsUInt32 i = 0; i < bf.GetCount(); ++i)
      NS_TEST_BOOL(!bf.IsBitSet(i));


    bf.SetCount(349, true);
    NS_TEST_INT(bf.GetCount(), 349);

    for (nsUInt32 i = 0; i < 249; ++i)
      NS_TEST_BOOL(!bf.IsBitSet(i));

    for (nsUInt32 i = 249; i < bf.GetCount(); ++i)
      NS_TEST_BOOL(bf.IsBitSet(i));
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "SetCount / SetBit / ClearBit / SetBitValue / SetCountUninitialized")
  {
    nsHybridBitfield<512> bf; // using a hybrid array

    bf.SetCount(100, false);
    NS_TEST_INT(bf.GetCount(), 100);

    for (nsUInt32 i = 0; i < bf.GetCount(); ++i)
      NS_TEST_BOOL(!bf.IsBitSet(i));

    bf.SetCount(200, true);
    NS_TEST_INT(bf.GetCount(), 200);

    for (nsUInt32 i = 100; i < bf.GetCount(); ++i)
      NS_TEST_BOOL(bf.IsBitSet(i));

    bf.SetCountUninitialized(250);
    NS_TEST_INT(bf.GetCount(), 250);

    bf.ClearAllBits();

    for (nsUInt32 i = 0; i < bf.GetCount(); i += 2)
      bf.SetBit(i);

    for (nsUInt32 i = 0; i < bf.GetCount(); i += 2)
    {
      NS_TEST_BOOL(bf.IsBitSet(i));
      NS_TEST_BOOL(!bf.IsBitSet(i + 1));
    }

    for (nsUInt32 i = 0; i < bf.GetCount(); i += 2)
    {
      bf.ClearBit(i);
      bf.SetBit(i + 1);
    }

    for (nsUInt32 i = 0; i < bf.GetCount(); i += 2)
    {
      NS_TEST_BOOL(!bf.IsBitSet(i));
      NS_TEST_BOOL(bf.IsBitSet(i + 1));
    }

    for (nsUInt32 i = 0; i < bf.GetCount(); ++i)
    {
      bf.SetBitValue(i, (i % 3) == 0);
    }

    for (nsUInt32 i = 0; i < bf.GetCount(); ++i)
    {
      NS_TEST_BOOL(bf.IsBitSet(i) == ((i % 3) == 0));
    }
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "SetBitRange")
  {
    for (nsUInt32 size = 1; size < 1024; ++size)
    {
      nsBitfield<nsDeque<nsUInt32>> bf; // using a deque
      bf.SetCount(size, false);

      NS_TEST_INT(bf.GetCount(), size);

      for (nsUInt32 count = 0; count < bf.GetCount(); ++count)
        NS_TEST_BOOL(!bf.IsBitSet(count));

      nsUInt32 uiStart = size / 2;
      nsUInt32 uiEnd = nsMath::Min(uiStart + (size / 3 * 2), size - 1);

      bf.SetBitRange(uiStart, uiEnd - uiStart + 1);

      for (nsUInt32 count = 0; count < uiStart; ++count)
        NS_TEST_BOOL(!bf.IsBitSet(count));
      for (nsUInt32 count = uiStart; count <= uiEnd; ++count)
        NS_TEST_BOOL(bf.IsBitSet(count));
      for (nsUInt32 count = uiEnd + 1; count < bf.GetCount(); ++count)
        NS_TEST_BOOL(!bf.IsBitSet(count));
    }
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "ClearBitRange")
  {
    for (nsUInt32 size = 1; size < 1024; ++size)
    {
      nsBitfield<nsDeque<nsUInt32>> bf; // using a deque
      bf.SetCount(size, true);

      NS_TEST_INT(bf.GetCount(), size);

      for (nsUInt32 count = 0; count < bf.GetCount(); ++count)
        NS_TEST_BOOL(bf.IsBitSet(count));

      nsUInt32 uiStart = size / 2;
      nsUInt32 uiEnd = nsMath::Min(uiStart + (size / 3 * 2), size - 1);

      bf.ClearBitRange(uiStart, uiEnd - uiStart + 1);

      for (nsUInt32 count = 0; count < uiStart; ++count)
        NS_TEST_BOOL(bf.IsBitSet(count));
      for (nsUInt32 count = uiStart; count <= uiEnd; ++count)
        NS_TEST_BOOL(!bf.IsBitSet(count));
      for (nsUInt32 count = uiEnd + 1; count < bf.GetCount(); ++count)
        NS_TEST_BOOL(bf.IsBitSet(count));
    }
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "IsAnyBitSet / IsNoBitSet / AreAllBitsSet")
  {
    nsHybridBitfield<512> bf; // using a hybrid array

    NS_TEST_BOOL(bf.IsEmpty() == true);
    NS_TEST_BOOL(bf.IsAnyBitSet() == false); // empty
    NS_TEST_BOOL(bf.IsNoBitSet() == true);
    NS_TEST_BOOL(bf.AreAllBitsSet() == false); // empty

    bf.SetCount(250, false);

    NS_TEST_BOOL(bf.IsEmpty() == false);
    NS_TEST_BOOL(bf.IsAnyBitSet() == false);
    NS_TEST_BOOL(bf.IsNoBitSet() == true);
    NS_TEST_BOOL(bf.AreAllBitsSet() == false);

    for (nsUInt32 i = 0; i < bf.GetCount(); i += 2)
      bf.SetBit(i);

    NS_TEST_BOOL(bf.IsEmpty() == false);
    NS_TEST_BOOL(bf.IsAnyBitSet() == true);
    NS_TEST_BOOL(bf.IsNoBitSet() == false);
    NS_TEST_BOOL(bf.AreAllBitsSet() == false);

    for (nsUInt32 i = 0; i < bf.GetCount(); i++)
      bf.SetBit(i);

    NS_TEST_BOOL(bf.IsAnyBitSet() == true);
    NS_TEST_BOOL(bf.IsNoBitSet() == false);
    NS_TEST_BOOL(bf.AreAllBitsSet() == true);
  }
}


NS_CREATE_SIMPLE_TEST(Containers, StaticBitfield)
{
  NS_TEST_BLOCK(nsTestBlock::Enabled, "SetAllBits / ClearAllBits")
  {
    nsStaticBitfield64 bf;

    for (nsUInt32 i = 0; i < bf.GetStorageTypeBitCount(); ++i)
      NS_TEST_BOOL(!bf.IsBitSet(i));

    bf.SetAllBits();

    for (nsUInt32 i = 0; i < bf.GetStorageTypeBitCount(); ++i)
      NS_TEST_BOOL(bf.IsBitSet(i));

    bf.ClearAllBits();

    for (nsUInt32 i = 0; i < bf.GetStorageTypeBitCount(); ++i)
      NS_TEST_BOOL(!bf.IsBitSet(i));
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "SetBit / ClearBit / SetBitValue")
  {
    nsStaticBitfield32 bf;

    for (nsUInt32 i = 0; i < bf.GetStorageTypeBitCount(); ++i)
      NS_TEST_BOOL(!bf.IsBitSet(i));

    for (nsUInt32 i = 0; i < bf.GetStorageTypeBitCount(); i += 2)
      bf.SetBit(i);

    for (nsUInt32 i = 0; i < bf.GetStorageTypeBitCount(); i += 2)
    {
      NS_TEST_BOOL(bf.IsBitSet(i));
      NS_TEST_BOOL(!bf.IsBitSet(i + 1));
    }

    for (nsUInt32 i = 0; i < bf.GetStorageTypeBitCount(); i += 2)
    {
      bf.ClearBit(i);
      bf.SetBit(i + 1);
    }

    for (nsUInt32 i = 0; i < bf.GetStorageTypeBitCount(); i += 2)
    {
      NS_TEST_BOOL(!bf.IsBitSet(i));
      NS_TEST_BOOL(bf.IsBitSet(i + 1));
    }

    for (nsUInt32 i = 0; i < bf.GetStorageTypeBitCount(); ++i)
    {
      bf.SetBitValue(i, (i % 3) == 0);
    }

    for (nsUInt32 i = 0; i < bf.GetStorageTypeBitCount(); ++i)
    {
      NS_TEST_BOOL(bf.IsBitSet(i) == ((i % 3) == 0));
    }
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "SetBitRange")
  {
    for (nsUInt32 uiStart = 0; uiStart < 61; ++uiStart)
    {
      nsStaticBitfield64 bf;

      for (nsUInt32 count = 0; count < bf.GetStorageTypeBitCount(); ++count)
        NS_TEST_BOOL(!bf.IsBitSet(count));

      nsUInt32 uiEnd = uiStart + 3;

      bf.SetBitRange(uiStart, uiEnd - uiStart + 1);

      for (nsUInt32 count = 0; count < uiStart; ++count)
        NS_TEST_BOOL(!bf.IsBitSet(count));
      for (nsUInt32 count = uiStart; count <= uiEnd; ++count)
        NS_TEST_BOOL(bf.IsBitSet(count));
      for (nsUInt32 count = uiEnd + 1; count < bf.GetStorageTypeBitCount(); ++count)
        NS_TEST_BOOL(!bf.IsBitSet(count));
    }
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "ClearBitRange")
  {
    for (nsUInt32 uiStart = 0; uiStart < 61; ++uiStart)
    {
      nsStaticBitfield64 bf;
      bf.SetAllBits();

      for (nsUInt32 count = 0; count < bf.GetStorageTypeBitCount(); ++count)
        NS_TEST_BOOL(bf.IsBitSet(count));

      nsUInt32 uiEnd = uiStart + 3;

      bf.ClearBitRange(uiStart, uiEnd - uiStart + 1);

      for (nsUInt32 count = 0; count < uiStart; ++count)
        NS_TEST_BOOL(bf.IsBitSet(count));
      for (nsUInt32 count = uiStart; count <= uiEnd; ++count)
        NS_TEST_BOOL(!bf.IsBitSet(count));
      for (nsUInt32 count = uiEnd + 1; count < bf.GetStorageTypeBitCount(); ++count)
        NS_TEST_BOOL(bf.IsBitSet(count));
    }
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "IsAnyBitSet / IsNoBitSet / AreAllBitsSet")
  {
    nsStaticBitfield8 bf;

    NS_TEST_BOOL(bf.IsAnyBitSet() == false); // empty
    NS_TEST_BOOL(bf.IsNoBitSet() == true);
    NS_TEST_BOOL(bf.AreAllBitsSet() == false); // empty

    for (nsUInt32 i = 0; i < bf.GetStorageTypeBitCount(); i += 2)
      bf.SetBit(i);

    NS_TEST_BOOL(bf.IsAnyBitSet() == true);
    NS_TEST_BOOL(bf.IsNoBitSet() == false);
    NS_TEST_BOOL(bf.AreAllBitsSet() == false);

    for (nsUInt32 i = 0; i < bf.GetStorageTypeBitCount(); i++)
      bf.SetBit(i);

    NS_TEST_BOOL(bf.IsAnyBitSet() == true);
    NS_TEST_BOOL(bf.IsNoBitSet() == false);
    NS_TEST_BOOL(bf.AreAllBitsSet() == true);
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "GetNumBitsSet")
  {
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0).GetNumBitsSet(), 0);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0xff).GetNumBitsSet(), 8);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0xffff).GetNumBitsSet(), 16);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0xffffffffu).GetNumBitsSet(), 32);
    NS_TEST_INT(nsStaticBitfield64::MakeFromMask(0).GetNumBitsSet(), 0);
    NS_TEST_INT(nsStaticBitfield64::MakeFromMask(0xff).GetNumBitsSet(), 8);
    NS_TEST_INT(nsStaticBitfield64::MakeFromMask(0xffff).GetNumBitsSet(), 16);
    NS_TEST_INT(nsStaticBitfield64::MakeFromMask(0xffffffffu).GetNumBitsSet(), 32);
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "GetLowestBitSet")
  {
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0u).GetLowestBitSet(), 32);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(1u).GetLowestBitSet(), 0);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0xffu).GetLowestBitSet(), 0);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0xff00u).GetLowestBitSet(), 8);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0xff0000u).GetLowestBitSet(), 16);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0xff000000u).GetLowestBitSet(), 24);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0x80000000u).GetLowestBitSet(), 31);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0xffffffffu).GetLowestBitSet(), 0);
    NS_TEST_INT(nsStaticBitfield64::MakeFromMask(0xffffffffffffffffull).GetLowestBitSet(), 0);
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "GetHighestBitSet")
  {
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0u).GetHighestBitSet(), 32);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(1u).GetHighestBitSet(), 0);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0xffu).GetHighestBitSet(), 7);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0xff00u).GetHighestBitSet(), 15);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0xff0000u).GetHighestBitSet(), 23);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0xff000000u).GetHighestBitSet(), 31);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0x80000000u).GetHighestBitSet(), 31);
    NS_TEST_INT(nsStaticBitfield32::MakeFromMask(0xffffffffu).GetHighestBitSet(), 31);
    NS_TEST_INT(nsStaticBitfield64::MakeFromMask(0xffffffffffffffffull).GetHighestBitSet(), 63);
  }
}
