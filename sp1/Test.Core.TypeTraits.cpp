#include "gtest/gtest.h"
#include "rad/Core/TypeTraits.h"

TEST(Core, TypeTraits)
{
    size_t maxSize = rad::MaxSizeof<char, int, long, long long, int32_t, int64_t>::value;
    EXPECT_EQ(maxSize, 8);
}
