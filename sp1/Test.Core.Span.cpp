#include "gtest/gtest.h"
#include "rad/Core/Span.h"

TEST(Core, Span)
{
    std::array<int, 8> a1 = { 1, 2, 3, 4, 5, 6, 7, 8 };
    std::vector<int> a2 = { 1, 2, 3, 4, 5, 6, 7, 8 };

    rad::Span<int> s1 = a1;
    rad::Span<int> s2 = a2;

    s1 = s1.take_while([](int n) { return (n <= 4); });
    EXPECT_EQ(s1.size(), 4);
    if (s1.size() == 4)
    {
        EXPECT_EQ(s1[0], 1);
        EXPECT_EQ(s1[1], 2);
        EXPECT_EQ(s1[2], 3);
        EXPECT_EQ(s1[3], 4);
    }
    s2 = s2.drop_while([](int n) { return (n <= 4); });
    EXPECT_EQ(s2.size(), 4);
    if (s2.size() == 4)
    {
        EXPECT_EQ(s2[0], 5);
        EXPECT_EQ(s2[1], 6);
        EXPECT_EQ(s2[2], 7);
        EXPECT_EQ(s2[3], 8);
    }
}
