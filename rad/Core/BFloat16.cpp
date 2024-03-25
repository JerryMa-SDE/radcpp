#include "BFloat16.h"
#include <cmath>
#include <memory>

namespace rad
{

uint16_t FP32ToBF16(float x)
{
    union
    {
        float f32;
        uint32_t u32;
    };
    f32 = x;
    return uint16_t(u32 >> 16);
}

uint16_t FP32ToBF16Round(float x)
{
    return FP32ToBF16(x * 1.001957f);
}

// https://github.com/pytorch/pytorch/blob/main/c10/util/BFloat16.h
uint16_t FP32ToBF16RoundToNearestEven(float x)
{
    if (!std::isnan(x))
    {
        union
        {
            float f32;
            uint32_t u32;
        };
        f32 = x;
        uint32_t roundingBias = ((u32 >> 16) & 1) + UINT32_C(0x7FFF);
        return static_cast<uint16_t>((u32 + roundingBias) >> 16);
    }
    else
    {
        return UINT16_C(0x7FC0);
    }
}

float BF16ToFP32(uint16_t x)
{
    union
    {
        uint32_t u32;
        float f32;
    };
    u32 = x;
    u32 <<= 16;
    return f32;
}

} // namespace rad
