#include <gtest/gtest.h>
#include "rad/Core/Integer.h"

TEST(Core, Integer)
{
    EXPECT_TRUE(std::has_single_bit(0b1000u));
    EXPECT_FALSE(std::has_single_bit(0b1010u));
    EXPECT_TRUE(rad::IsPow2(0b1000u));
    EXPECT_FALSE(rad::IsPow2(0b1010u));
    EXPECT_EQ(std::bit_ceil(0b1010u), 0b10000u);
    EXPECT_EQ(std::bit_floor(0b1010u), 0b1000u);
    EXPECT_EQ(std::bit_width(0b1010u), 4u);
    EXPECT_EQ(std::rotl(uint16_t(0xABCD), 4), 0xBCDA);
    EXPECT_EQ(std::rotr(uint16_t(0xABCD), 4), 0xDABC);
    EXPECT_EQ(std::countl_zero(0b1010u), 28);
    EXPECT_EQ(std::countl_one(0xEEEEEEEEu), 3);
    EXPECT_EQ(std::countr_zero(0x88840u), 6);
    EXPECT_EQ(std::countr_one(0x777Fu), 7);
    EXPECT_EQ(std::popcount(0x5555u), 8);
    EXPECT_EQ(rad::CountBits(0xAAAAu), 8);
    EXPECT_EQ(rad::BitScanReverse32(0x5555u), 14);
    EXPECT_EQ(rad::BitScanReverse64(0x555500005555u), 46);
    EXPECT_EQ(rad::Pow2AlignUp(0u, 16u), 0u);
    EXPECT_EQ(rad::Pow2AlignUp(90u, 16u), 96u);
    EXPECT_EQ(rad::Pow2AlignDown(0u, 16u), 0u);
    EXPECT_EQ(rad::Pow2AlignDown(90u, 16u), 80u);
    EXPECT_EQ(rad::RoundUpToNextPow2(0u), 1u);
    EXPECT_EQ(rad::RoundUpToNextPow2(90u), 128u);
    EXPECT_EQ(rad::RoundUpToNextPow2(128u), 256u);
    EXPECT_EQ(rad::RoundUpToPow2(0u), 1u);
    EXPECT_EQ(rad::RoundUpToPow2(90u), 128u);
    EXPECT_EQ(rad::RoundUpToPow2(128u), 128u);
    EXPECT_EQ(rad::RoundUpToMultiple(0, 12), 0);
    EXPECT_EQ(rad::RoundUpToMultiple(90, 12), 96);
    EXPECT_EQ(rad::RoundDownToMultiple(0, 12), 0);
    EXPECT_EQ(rad::RoundDownToMultiple(90, 12), 84);
}
