#include "gtest/gtest.h"
#include "rad/Core/Span.h"
#include "boost/container/small_vector.hpp"

int Sum(rad::Span<int> numbers)
{
    int sum = 0;
    for (int n : numbers)
    {
        sum += n;
    }
    return sum;
}

TEST(Core, Span)
{
    int c1[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
    std::array<int, 8> c2 = { 1, 2, 3, 4, 5, 6, 7, 8 };
    std::vector<int> c3 = { c2.begin(), c2.end() };
    boost::container::small_vector<int, 8> c4 = { c2.begin(), c2.end() };

    int s = Sum(36); // single value
    EXPECT_EQ(s, 36);
    s = Sum({ 1, 2, 3, 4, 5, 6, 7, 8 }); // initializer list
    EXPECT_EQ(s, 36);
    s = Sum(c1);
    EXPECT_EQ(s, 36);
    s = Sum(c2);
    EXPECT_EQ(s, 36);
    s = Sum(c3);
    EXPECT_EQ(s, 36);
    s = Sum(c4);
    EXPECT_EQ(s, 36);

    rad::Span<int> s1 = c1;
    rad::Span<int> s2 = c2;

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
