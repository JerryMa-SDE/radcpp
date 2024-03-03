#include "gtest/gtest.h"
#include "rad/Core/Flags.h"

enum class FlagBits : uint32_t
{
    Bit0 = 1 << 0,
    Bit1 = 1 << 1,
    Bit2 = 1 << 2,
    Bit3 = 1 << 3,
    Bit4 = 1 << 4,
    Bit5 = 1 << 5,
    Bit6 = 1 << 6,
    Bit7 = 1 << 7,
};

TEST(Core, Flags)
{
    rad::Flags32<FlagBits> flags(0);
    flags |= FlagBits::Bit0;
    flags |= FlagBits::Bit1;
    flags |= FlagBits::Bit2;
    flags |= FlagBits::Bit3;
    flags |= FlagBits::Bit4;
    flags |= FlagBits::Bit5;
    flags |= FlagBits::Bit6;
    flags |= FlagBits::Bit7;
    EXPECT_EQ(flags, 0b11111111);
    flags ^= FlagBits::Bit0;
    flags ^= FlagBits::Bit2;
    flags ^= FlagBits::Bit4;
    flags ^= FlagBits::Bit6;
    EXPECT_EQ(flags, 0b10101010);
}
