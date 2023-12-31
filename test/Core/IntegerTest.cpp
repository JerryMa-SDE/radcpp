#include "gtest/gtest.h"
#include "rad/Core/Integer.h"

void TestCountBits()
{
    EXPECT_EQ(rad::CountBits(0x00000000), 0);
    EXPECT_EQ(rad::CountBits(0x00000008), 1);
    EXPECT_EQ(rad::CountBits(0x00008008), 2);
    EXPECT_EQ(rad::CountBits(0x80008008), 3);
    EXPECT_EQ(rad::CountBits(0x88888888), 8);
    EXPECT_EQ(rad::CountBits(0x0000FFFF), 16);
    EXPECT_EQ(rad::CountBits(0xFFFF0000), 16);
    EXPECT_EQ(rad::CountBits(0xF0F0F0F0), 16);
    EXPECT_EQ(rad::CountBits(0x0F0F0F0F), 16);
    EXPECT_EQ(rad::CountBits(0x55555555), 16);
    EXPECT_EQ(rad::CountBits(0xAAAAAAAA), 16);
    EXPECT_EQ(rad::CountBits(0xFFFFFFFF), 32);
}

void TestBitScanReverse()
{
    EXPECT_EQ(rad::BitScanReverse32(0x00000001), 0);
    EXPECT_EQ(rad::BitScanReverse32(0x0000000A), 3);
    EXPECT_EQ(rad::BitScanReverse32(0x000A000A), 19);
    EXPECT_EQ(rad::BitScanReverse32(0xA00A000A), 31);
    EXPECT_EQ(rad::BitScanReverse64(0x0000000000000001), 0);
    EXPECT_EQ(rad::BitScanReverse64(0x000000000000000A), 3);
    EXPECT_EQ(rad::BitScanReverse64(0x0000000A000A000A), 35);
    EXPECT_EQ(rad::BitScanReverse64(0xA00A000A000A000A), 63);
}

void TestPow2Align()
{
    EXPECT_EQ(rad::Pow2AlignUp(124, 8), 128);
    EXPECT_EQ(rad::Pow2AlignDown(124, 8), 120);
    EXPECT_EQ(rad::Pow2AlignUp(124, 32), 128);
    EXPECT_EQ(rad::Pow2AlignDown(124, 32), 96);
    EXPECT_EQ(rad::Pow2AlignUp(256, 256), 256);
    EXPECT_EQ(rad::Pow2AlignDown(256, 256), 256);
}

void TestRound()
{
    EXPECT_EQ(rad::RoundUpToNextPow2(uint32_t(0)), 1);
    EXPECT_EQ(rad::RoundUpToNextPow2(uint32_t(42)), 64);
    EXPECT_EQ(rad::RoundUpToNextPow2(uint32_t(64)), 128);
    EXPECT_EQ(rad::RoundUpToNextPow2(uint32_t(0x0FFFFFFF)), 0x10000000);

    EXPECT_EQ(rad::RoundUpToPow2(uint32_t(0)), 1);
    EXPECT_EQ(rad::RoundUpToPow2(uint32_t(42)), 64);
    EXPECT_EQ(rad::RoundUpToPow2(uint32_t(64)), 64);
    EXPECT_EQ(rad::RoundUpToPow2(uint32_t(0x0FFFFFFF)), 0x10000000);

    EXPECT_EQ(rad::RoundUpToMultiple(0, 12), 12);
    EXPECT_EQ(rad::RoundUpToMultiple(12, 12), 12);
    EXPECT_EQ(rad::RoundUpToMultiple(42, 12), 48);
    EXPECT_EQ(rad::RoundUpToMultiple(42, 96), 96);
    EXPECT_EQ(rad::RoundDownToMultiple(0, 12), 0);
    EXPECT_EQ(rad::RoundDownToMultiple(12, 12), 12);
    EXPECT_EQ(rad::RoundDownToMultiple(42, 12), 36);
    EXPECT_EQ(rad::RoundDownToMultiple(42, 96), 0);
}

TEST(Core, Integer)
{
    TestCountBits();
    TestBitScanReverse();
    TestPow2Align();
    TestRound();
}
