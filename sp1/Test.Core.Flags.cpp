#include "gtest/gtest.h"
#include "rad/Core/Flags.h"

enum class FlagBits : uint32_t
{
    F0 = 1 << 0,
    F1 = 1 << 1,
    F2 = 1 << 2,
    F3 = 1 << 3,
};

TEST(Core, Flags)
{
    rad::Flags32<FlagBits> flags(0);
    flags |= FlagBits::F0;
    flags |= FlagBits::F1;
    flags |= FlagBits::F2;
    flags |= FlagBits::F3;
    EXPECT_EQ(flags.GetMask(), 0xF);
}
