#include "gtest/gtest.h"
#include "rad/Core/Math.h"

TEST(Core, Math)
{
    float t0 = 0.0f;
    float t1 = 0.0f;
    EXPECT_FALSE(rad::SolveQuadraticEquation(1, 1, 1, t0, t1));
    t0 = 0.0f;
    t1 = 0.0f;
    EXPECT_TRUE(rad::SolveQuadraticEquation(1, 2, -3, t0, t1));
    EXPECT_EQ(t0, -3.0f);
    EXPECT_EQ(t1, +1.0f);
    t0 = 0.0f;
    t1 = 0.0f;
    EXPECT_TRUE(rad::SolveQuadraticEquation(1, 10, 16, t0, t1));
    EXPECT_EQ(t0, -8.0f);
    EXPECT_EQ(t1, -2.0f);
    t0 = 0.0f;
    t1 = 0.0f;
    EXPECT_TRUE(rad::SolveQuadraticEquation(3, -12, 12, t0, t1));
    EXPECT_EQ(t0, +2.0f);
    EXPECT_EQ(t1, +2.0f);
    t0 = 0.0f;
    t1 = 0.0f;
    EXPECT_TRUE(rad::SolveQuadraticEquation(3, 4, 1, t0, t1));
    EXPECT_EQ(t0, -1.0f);
    EXPECT_EQ(t1, -1.0f / 3.0f);
}
