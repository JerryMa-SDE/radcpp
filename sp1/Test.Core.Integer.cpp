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
}
