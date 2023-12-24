#include "gtest/gtest.h"
#include "rad/Core/Float.h"
#include "rad/IO/Logging.h"

float g_maxEpsilonU8 = 0.0f;
float g_maxEpsilonU16 = 0.0f;
float g_maxEpsilonU32 = 0.0f;

void TestFloatQuantizeUnorm8(float normalized)
{
    float f32 = normalized;
    uint8_t u8 = rad::QuantizeUnorm8(f32);
    f32 = rad::DequantizeUnorm8(u8);
    float epsilon = std::abs(f32 - normalized);
    EXPECT_TRUE(epsilon < 0.002f);
    for (size_t i = 0; i < 1000; ++i)
    {
        u8 = rad::QuantizeUnorm8(f32);
        f32 = rad::DequantizeUnorm8(u8);
        EXPECT_TRUE(std::abs(f32 - normalized) <= epsilon);
    }
    if (g_maxEpsilonU8 < epsilon)
    {
        g_maxEpsilonU8 = epsilon;
    }
}

void TestFloatQuantizeUnorm16(float normalized)
{
    float f32 = normalized;
    uint16_t u16 = rad::QuantizeUnorm16(f32);
    f32 = rad::DequantizeUnorm16(u16);
    float epsilon = std::abs(f32 - normalized);
    EXPECT_TRUE(epsilon < 0.0001f);
    for (size_t i = 0; i < 1000; ++i)
    {
        u16 = rad::QuantizeUnorm16(f32);
        f32 = rad::DequantizeUnorm16(u16);
        EXPECT_TRUE(std::abs(f32 - normalized) <= epsilon);
    }
    if (g_maxEpsilonU16 < epsilon)
    {
        g_maxEpsilonU16 = epsilon;
    }
}

void TestFloatQuantizeUnorm32(float normalized)
{
    float f32 = normalized;
    uint32_t u32 = rad::QuantizeUnorm32(f32);
    f32 = rad::DequantizeUnorm32(u32);
    float epsilon = std::abs(f32 - normalized);
    EXPECT_TRUE(epsilon < 0.000001f);
    for (size_t i = 0; i < 1000; ++i)
    {
        u32 = rad::QuantizeUnorm32(f32);
        f32 = rad::DequantizeUnorm32(u32);
        EXPECT_TRUE(std::abs(f32 - normalized) <= epsilon);
    }
    if (g_maxEpsilonU32 < epsilon)
    {
        g_maxEpsilonU32 = epsilon;
    }
}

void TestFloatQuantize(float normalized)
{
    TestFloatQuantizeUnorm8(normalized);
    TestFloatQuantizeUnorm16(normalized);
    TestFloatQuantizeUnorm32(normalized);
}

TEST(Core, Float)
{
    float f32 = 0.0f;
    uint8_t u8 = rad::QuantizeUnorm8(f32);
    EXPECT_EQ(u8, 0);
    EXPECT_EQ(f32, rad::DequantizeUnorm8(u8));
    uint16_t u16 = rad::QuantizeUnorm16(f32);
    EXPECT_EQ(u16, 0);
    EXPECT_EQ(f32, rad::DequantizeUnorm16(u16));
    uint32_t u32 = rad::QuantizeUnorm32(f32);
    EXPECT_EQ(u32, 0);
    EXPECT_EQ(f32, rad::DequantizeUnorm32(u32));

    f32 = 0.5f;
    u8 = rad::QuantizeUnorm8(f32);
    EXPECT_EQ(u8, UINT8_MAX / 2 + 1);
    u16 = rad::QuantizeUnorm16(f32);
    EXPECT_EQ(u16, UINT16_MAX / 2 + 1);
    u32 = rad::QuantizeUnorm32(f32);
    EXPECT_EQ(u32, UINT32_MAX / 2 + 1);

    f32 = 1.0f;
    u8 = rad::QuantizeUnorm8(f32);
    EXPECT_EQ(u8, UINT8_MAX);
    EXPECT_EQ(f32, rad::DequantizeUnorm8(u8));
    u16 = rad::QuantizeUnorm16(f32);
    EXPECT_EQ(u16, UINT16_MAX);
    EXPECT_EQ(f32, rad::DequantizeUnorm16(u16));
    u32 = rad::QuantizeUnorm32(f32);
    EXPECT_EQ(u32, UINT32_MAX);
    EXPECT_EQ(f32, rad::DequantizeUnorm32(u32));

    for (float f = 0.0f; f <= 1.0f; f += 0.0001f)
    {
        TestFloatQuantize(f);
    }

    LogGlobal(Info, "Max Epsilon Quantize/DequantizeUnorm8: %f", g_maxEpsilonU8);
    LogGlobal(Info, "Max Epsilon Quantize/DequantizeUnorm16: %f", g_maxEpsilonU16);
    LogGlobal(Info, "Max Epsilon Quantize/DequantizeUnorm32: %f", g_maxEpsilonU32);
}
