#include "gtest/gtest.h"
#include "rad/Core/TypeTraits.h"

TEST(Core, TypeTraits)
{
    size_t maxSize = rad::MaxSizeof<int8_t, int64_t, int16_t, int32_t>::value;
    EXPECT_EQ(maxSize, 8);
}
