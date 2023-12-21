#include "gtest/gtest.h"
#include "rad/Core/Float.h"

void TestFloatQuantizeUnorm8(float normalized)
{
    float f32 = normalized;
    uint8_t u8 = rad::QuantizeUnorm8(f32);
    f32 = rad::DequantizeUnorm8(u8);
    float epsilon = f32 - normalized;
    for (size_t i = 0; i < 1000; ++i)
    {
        u8 = rad::QuantizeUnorm8(f32);
        f32 = rad::DequantizeUnorm8(u8);
        EXPECT_TRUE(epsilon >= f32 - normalized);
    }
    EXPECT_TRUE(f32 - normalized < 0.01f);
}

void TestFloatQuantizeUnorm16(float normalized)
{
    float f32 = normalized;
    uint16_t u16 = rad::QuantizeUnorm16(f32);
    f32 = rad::DequantizeUnorm16(u16);
    float epsilon = f32 - normalized;
    for (size_t i = 0; i < 1000; ++i)
    {
        u16 = rad::QuantizeUnorm16(f32);
        f32 = rad::DequantizeUnorm16(u16);
        EXPECT_TRUE(epsilon >= f32 - normalized);
    }
    EXPECT_TRUE(f32 - normalized < 0.01f);
}

void TestFloatQuantizeUnorm32(float normalized)
{
    float f32 = normalized;
    uint32_t u32 = rad::QuantizeUnorm32(f32);
    f32 = rad::DequantizeUnorm32(u32);
    float epsilon = f32 - normalized;
    for (size_t i = 0; i < 1000; ++i)
    {
        u32 = rad::QuantizeUnorm32(f32);
        f32 = rad::DequantizeUnorm32(u32);
        EXPECT_TRUE(epsilon >= f32 - normalized);
    }
    EXPECT_TRUE(f32 - normalized < 0.01f);
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

    for (uint32_t i = 0; i < 256; ++i)
    {
        float normalized = rad::DequantizeUnorm8(uint8_t(i));
        TestFloatQuantize(normalized);
    }
}
