/*
 *   Copyright (c) 2023-present WD Studios L.L.C.
 *   All rights reserved.
 *   You are only allowed access to this code, if given WRITTEN permission by Watch Dogs LLC.
 */
#include <FoundationTest/FoundationTestPCH.h>

namespace
{
  // declare bitflags using macro magic
  NS_DECLARE_FLAGS(nsUInt32, AutoFlags, Bit1, Bit2, Bit3, Bit4);

  // declare bitflags manually
  struct ManualFlags
  {
    using StorageType = nsUInt32;

    enum Enum
    {
      Bit1 = NS_BIT(0),
      Bit2 = NS_BIT(1),
      Bit3 = NS_BIT(2),
      Bit4 = NS_BIT(3),

      Default = Bit1 | Bit2
    };

    struct Bits
    {
      StorageType Bit1 : 1;
      StorageType Bit2 : 1;
      StorageType Bit3 : 1;
      StorageType Bit4 : 1;
    };
  };

  NS_DECLARE_FLAGS_OPERATORS(ManualFlags);
} // namespace

NS_CHECK_AT_COMPILETIME(sizeof(nsBitflags<AutoFlags>) == 4);

NS_CREATE_SIMPLE_TEST(Basics, Bitflags)
{
  NS_TEST_BOOL(AutoFlags::Count == 4);

  {
    nsBitflags<AutoFlags> flags = AutoFlags::Bit1 | AutoFlags::Bit4;

    NS_TEST_BOOL(flags.IsSet(AutoFlags::Bit4));
    NS_TEST_BOOL(flags.AreAllSet(AutoFlags::Bit1 | AutoFlags::Bit4));
    NS_TEST_BOOL(flags.IsAnySet(AutoFlags::Bit1 | AutoFlags::Bit2));
    NS_TEST_BOOL(!flags.IsAnySet(AutoFlags::Bit2 | AutoFlags::Bit3));
    NS_TEST_BOOL(flags.AreNoneSet(AutoFlags::Bit2 | AutoFlags::Bit3));
    NS_TEST_BOOL(!flags.AreNoneSet(AutoFlags::Bit2 | AutoFlags::Bit4));

    flags.Add(AutoFlags::Bit3);
    NS_TEST_BOOL(flags.IsSet(AutoFlags::Bit3));

    flags.Remove(AutoFlags::Bit1);
    NS_TEST_BOOL(!flags.IsSet(AutoFlags::Bit1));

    flags.Toggle(AutoFlags::Bit4);
    NS_TEST_BOOL(flags.AreAllSet(AutoFlags::Bit3));

    flags.AddOrRemove(AutoFlags::Bit2, true);
    flags.AddOrRemove(AutoFlags::Bit3, false);
    NS_TEST_BOOL(flags.AreAllSet(AutoFlags::Bit2));

    flags.Add(AutoFlags::Bit1);

    nsBitflags<ManualFlags> manualFlags = ManualFlags::Default;
    NS_TEST_BOOL(manualFlags.AreAllSet(ManualFlags::Bit1 | ManualFlags::Bit2));
    NS_TEST_BOOL(manualFlags.GetValue() == flags.GetValue());
    NS_TEST_BOOL(manualFlags.AreAllSet(ManualFlags::Default & ManualFlags::Bit2));

    NS_TEST_BOOL(flags.IsAnyFlagSet());
    flags.Clear();
    NS_TEST_BOOL(flags.IsNoFlagSet());
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "operator&")
  {
    nsBitflags<AutoFlags> flags2 = AutoFlags::Bit1 & AutoFlags::Bit4;
    NS_TEST_BOOL(flags2.GetValue() == 0);
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "SetValue")
  {
    nsBitflags<AutoFlags> flags;
    flags.SetValue(17);
    NS_TEST_BOOL(flags.GetValue() == 17);
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "operator|=")
  {
    nsBitflags<AutoFlags> f = AutoFlags::Bit1 | AutoFlags::Bit2;
    f |= AutoFlags::Bit3;

    NS_TEST_BOOL(f.GetValue() == (AutoFlags::Bit1 | AutoFlags::Bit2 | AutoFlags::Bit3).GetValue());
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "operator&=")
  {
    nsBitflags<AutoFlags> f = AutoFlags::Bit1 | AutoFlags::Bit2 | AutoFlags::Bit3;
    f &= AutoFlags::Bit3;

    NS_TEST_BOOL(f.GetValue() == AutoFlags::Bit3);
  }
}


//////////////////////////////////////////////////////////////////////////

namespace
{
  struct TypelessFlags1
  {
    enum Enum
    {
      Bit1 = NS_BIT(0),
      Bit2 = NS_BIT(1),
    };
  };

  struct TypelessFlags2
  {
    enum Enum
    {
      Bit3 = NS_BIT(2),
      Bit4 = NS_BIT(3),
    };
  };
} // namespace

NS_CREATE_SIMPLE_TEST(Basics, TypelessBitflags)
{
  {
    nsTypelessBitflags<nsUInt32> flags = TypelessFlags1::Bit1 | TypelessFlags2::Bit4;

    NS_TEST_BOOL(flags.IsAnySet(TypelessFlags2::Bit4));
    NS_TEST_BOOL(flags.AreAllSet(TypelessFlags1::Bit1 | TypelessFlags2::Bit4));
    NS_TEST_BOOL(flags.IsAnySet(TypelessFlags1::Bit1 | TypelessFlags1::Bit2));
    NS_TEST_BOOL(!flags.IsAnySet(TypelessFlags1::Bit2 | TypelessFlags2::Bit3));
    NS_TEST_BOOL(flags.AreNoneSet(TypelessFlags1::Bit2 | TypelessFlags2::Bit3));
    NS_TEST_BOOL(!flags.AreNoneSet(TypelessFlags1::Bit2 | TypelessFlags2::Bit4));

    flags.Add(TypelessFlags2::Bit3);
    NS_TEST_BOOL(flags.IsAnySet(TypelessFlags2::Bit3));

    flags.Remove(TypelessFlags1::Bit1);
    NS_TEST_BOOL(!flags.IsAnySet(TypelessFlags1::Bit1));

    flags.Toggle(TypelessFlags2::Bit4);
    NS_TEST_BOOL(flags.AreAllSet(TypelessFlags2::Bit3));

    flags.AddOrRemove(TypelessFlags1::Bit2, true);
    flags.AddOrRemove(TypelessFlags2::Bit3, false);
    NS_TEST_BOOL(flags.AreAllSet(TypelessFlags1::Bit2));

    NS_TEST_BOOL(!flags.IsNoFlagSet());
    flags.Clear();
    NS_TEST_BOOL(flags.IsNoFlagSet());
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "operator&")
  {
    nsTypelessBitflags<nsUInt32> flags2 = TypelessFlags1::Bit1 & TypelessFlags2::Bit4;
    NS_TEST_BOOL(flags2.GetValue() == 0);
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "SetValue")
  {
    nsTypelessBitflags<nsUInt32> flags;
    NS_TEST_BOOL(flags.IsNoFlagSet());
    NS_TEST_BOOL(flags.GetValue() == 0);
    flags.SetValue(17);
    NS_TEST_BOOL(flags.GetValue() == 17);
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "operator|=")
  {
    nsTypelessBitflags<nsUInt32> f = TypelessFlags1::Bit1 | TypelessFlags1::Bit2;
    f |= TypelessFlags2::Bit3;

    NS_TEST_BOOL(f.GetValue() == (TypelessFlags1::Bit1 | TypelessFlags1::Bit2 | TypelessFlags2::Bit3));
  }

  NS_TEST_BLOCK(nsTestBlock::Enabled, "operator&=")
  {
    nsTypelessBitflags<nsUInt32> f = TypelessFlags1::Bit1 | TypelessFlags1::Bit2 | TypelessFlags2::Bit3;
    f &= TypelessFlags2::Bit3;

    NS_TEST_BOOL(f.GetValue() == TypelessFlags2::Bit3);
  }
}
