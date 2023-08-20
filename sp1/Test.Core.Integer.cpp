#include "gtest/gtest.h"
#include "rad/Core/Integer.h"

TEST(Core, Integer)
{
    EXPECT_EQ(rad::CountBits(0x00000000), 0);
    EXPECT_EQ(rad::CountBits(0x55555555), 16);
    EXPECT_EQ(rad::CountBits(0xAAAAAAAA), 16);
    EXPECT_EQ(rad::CountBits(0xFFFFFFFF), 32);

    EXPECT_EQ(rad::BitScanReverse32(0x00000001), 0);
    EXPECT_EQ(rad::BitScanReverse32(0x0000000F), 3);
    EXPECT_EQ(rad::BitScanReverse32(0x000F000F), 19);
    EXPECT_EQ(rad::BitScanReverse32(0xF00F000F), 31);
    EXPECT_EQ(rad::BitScanReverse64(0x0000000000000001), 0);
    EXPECT_EQ(rad::BitScanReverse64(0x000000000000000F), 3);
    EXPECT_EQ(rad::BitScanReverse64(0x0000000F0000000F), 35);
    EXPECT_EQ(rad::BitScanReverse64(0xF000000F0000000F), 63);

    EXPECT_TRUE(rad::IsPow2(1));
    EXPECT_TRUE(rad::IsPow2(32));
    EXPECT_TRUE(rad::IsPow2(64));
    EXPECT_TRUE(rad::IsPow2(128));
    EXPECT_TRUE(rad::IsPow2(256 * 256));
    EXPECT_TRUE(rad::IsPow2(1024ui64 * 1024ui64 * 1024ui64 * 1024ui64));
    EXPECT_FALSE(rad::IsPow2(0));
    EXPECT_FALSE(rad::IsPow2(9));
    EXPECT_FALSE(rad::IsPow2(18));
    EXPECT_FALSE(rad::IsPow2(25));
    EXPECT_FALSE(rad::IsPow2(33));
    EXPECT_FALSE(rad::IsPow2(42));
    EXPECT_FALSE(rad::IsPow2(0xFFFFFFFFui32));
    EXPECT_FALSE(rad::IsPow2(0xFFFFFFFFui64));

    EXPECT_EQ(rad::Pow2AlignUp(127, 8), 128);
    EXPECT_EQ(rad::Pow2AlignDown(127, 8), 120);
    EXPECT_EQ(rad::Pow2AlignUp(127, 32), 128);
    EXPECT_EQ(rad::Pow2AlignDown(127, 32), 96);
    EXPECT_EQ(rad::Pow2AlignUp(0xFFFFFFFFui32, 64), 0);
    EXPECT_EQ(rad::Pow2AlignUp(0xFFFFFFFFui64, 64), 0x100000000);
    EXPECT_EQ(rad::Pow2AlignDown(0xFFFFFFFF, 64), 0x100000000 - 64);

    EXPECT_EQ(rad::RoundUpToNextPow2(0ui32), 1);
    EXPECT_EQ(rad::RoundUpToNextPow2(42ui32), 64);
    EXPECT_EQ(rad::RoundUpToNextPow2(64ui32), 128);
    EXPECT_EQ(rad::RoundUpToNextPow2(0x0FFFFFFFui32), 0x10000000ui32);
    EXPECT_EQ(rad::RoundUpToNextPow2(0ui64), 1);
    EXPECT_EQ(rad::RoundUpToNextPow2(0xFFFFFFFFui64), 0x100000000ui64);
    EXPECT_EQ(rad::RoundUpToNextPow2(1024ui64 * 1024ui64 * 1024ui64 * 1024ui64), 1024ui64 * 1024ui64 * 1024ui64 * 1024ui64 * 2);
    EXPECT_EQ(rad::RoundUpToNextPow2(0x7FFFFFFFFFFFFFFFui64), 0x8000000000000000ui64);

    EXPECT_EQ(rad::RoundUpToPow2(0ui32), 1);
    EXPECT_EQ(rad::RoundUpToPow2(42ui32), 64);
    EXPECT_EQ(rad::RoundUpToPow2(64ui32), 64);
    EXPECT_EQ(rad::RoundUpToPow2(0x0FFFFFFFui32), 0x10000000ui32);
    EXPECT_EQ(rad::RoundUpToPow2(0ui64), 1);
    EXPECT_EQ(rad::RoundUpToPow2(0xFFFFFFFFui64), 0x100000000ui64);
    EXPECT_EQ(rad::RoundUpToPow2(1024ui64 * 1024ui64 * 1024ui64 * 1024ui64), 1024ui64 * 1024ui64 * 1024ui64 * 1024ui64);
    EXPECT_EQ(rad::RoundUpToPow2(0x7FFFFFFFFFFFFFFFui64), 0x8000000000000000ui64);
}
