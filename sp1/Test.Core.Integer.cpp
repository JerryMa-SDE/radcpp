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
    uint64_t bigNumber64 = uint64_t(1024) * uint64_t(1024) * uint64_t(1024) * uint64_t(1024);
    EXPECT_TRUE(rad::IsPow2(bigNumber64));
    EXPECT_FALSE(rad::IsPow2(0));
    EXPECT_FALSE(rad::IsPow2(9));
    EXPECT_FALSE(rad::IsPow2(18));
    EXPECT_FALSE(rad::IsPow2(25));
    EXPECT_FALSE(rad::IsPow2(33));
    EXPECT_FALSE(rad::IsPow2(42));
    EXPECT_FALSE(rad::IsPow2(0xFFFFFFFF));
    EXPECT_FALSE(rad::IsPow2(bigNumber64 / 3));
    EXPECT_FALSE(rad::IsPow2(0xFFFFFFFFFFFFFFFF));

    EXPECT_EQ(rad::Pow2AlignUp(124, 8), 128);
    EXPECT_EQ(rad::Pow2AlignDown(124, 8), 120);
    EXPECT_EQ(rad::Pow2AlignUp(124, 32), 128);
    EXPECT_EQ(rad::Pow2AlignDown(124, 32), 96);
    EXPECT_EQ(rad::Pow2AlignUp(256, 256), 256);
    EXPECT_EQ(rad::Pow2AlignDown(256, 256), 256);
    EXPECT_EQ(rad::Pow2AlignDown(0xFFFF, 64), 0xFFFF / 64 * 64);
    EXPECT_EQ(rad::Pow2AlignUp(uint64_t(0xFFFFFFFF), 64), 0x100000000);

    EXPECT_EQ(rad::RoundUpToNextPow2(uint32_t(0)), 1);
    EXPECT_EQ(rad::RoundUpToNextPow2(uint32_t(42)), 64);
    EXPECT_EQ(rad::RoundUpToNextPow2(uint32_t(64)), 128);
    EXPECT_EQ(rad::RoundUpToNextPow2(uint32_t(0x0FFFFFFF)), 0x10000000);
    EXPECT_EQ(rad::RoundUpToNextPow2(uint32_t(0)), 1);
    EXPECT_EQ(rad::RoundUpToNextPow2(bigNumber64), bigNumber64 * 2);
    EXPECT_EQ(rad::RoundUpToNextPow2(uint64_t(0x7FFFFFFFFFFFFFFF)), 0x8000000000000000);

    EXPECT_EQ(rad::RoundUpToPow2(uint32_t(0)), 1);
    EXPECT_EQ(rad::RoundUpToPow2(uint32_t(42)), 64);
    EXPECT_EQ(rad::RoundUpToPow2(uint32_t(64)), 64);
    EXPECT_EQ(rad::RoundUpToPow2(uint32_t(0x0FFFFFFF)), 0x10000000);
    EXPECT_EQ(rad::RoundUpToPow2(uint64_t(0)), 1);
    EXPECT_EQ(rad::RoundUpToPow2(uint64_t(0xFFFFFFFF)), 0x100000000);
    EXPECT_EQ(rad::RoundUpToPow2(bigNumber64), bigNumber64);
    EXPECT_EQ(rad::RoundUpToPow2(uint64_t(0x7FFFFFFFFFFFFFFF)), 0x8000000000000000);

    EXPECT_EQ(rad::RoundUpToMultiple(0, 12), 0);
    EXPECT_EQ(rad::RoundUpToMultiple(12, 12), 12);
    EXPECT_EQ(rad::RoundUpToMultiple(42, 12), 48);
    EXPECT_EQ(rad::RoundUpToMultiple(42, 96), 96);
    EXPECT_EQ(rad::RoundDownToMultiple(0, 12), 0);
    EXPECT_EQ(rad::RoundDownToMultiple(12, 12), 12);
    EXPECT_EQ(rad::RoundDownToMultiple(42, 12), 36);
    EXPECT_EQ(rad::RoundDownToMultiple(42, 96), 0);
}
