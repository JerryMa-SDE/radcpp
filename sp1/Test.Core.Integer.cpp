#include "gtest/gtest.h"
#include "rad/Core/Integer.h"

TEST(Core, Integer)
{
    EXPECT_EQ(rad::CountBits(0x00000000), 0);
    EXPECT_EQ(rad::CountBits(0x55555555), 16);
    EXPECT_EQ(rad::CountBits(0xAAAAAAAA), 16);
    EXPECT_EQ(rad::CountBits(0xFFFFFFFF), 32);
}
