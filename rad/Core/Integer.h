#pragma once

#include "Global.h"
#include <cassert>
#include <cstddef>
#include <cstdint>

namespace rad
{

using Int8 = int8_t;
using Int16 = int16_t;
using Int32 = int32_t;
using Int64 = int64_t;
using Uint8 = uint8_t;
using Uint16 = uint16_t;
using Uint32 = uint32_t;
using Uint64 = uint64_t;

/// Count the number of bits set in an unsigned integer.
uint32_t CountBits(uint32_t x);

/// Search the mask data from most significant bit (MSB) to least significant bit (LSB) for a set bit (1).
/// The mask must be nonzero or the index returned is undefined.
uint32_t BitScanReverse32(uint32_t mask);

/// Search the mask data from most significant bit (MSB) to least significant bit (LSB) for a set bit (1).
/// The mask must be nonzero or the index returned is undefined.
uint32_t BitScanReverse64(uint64_t mask);

constexpr bool IsPow2(uint64_t x)
{
    return (x == 0) ? false : ((x & (x - 1)) == 0);
}

template <typename T>
T Pow2AlignUp(T value, uint64_t alignment)
{
    assert(IsPow2(alignment));
    T res = ((value + static_cast<T>(alignment) - 1) & ~(static_cast<T>(alignment) - 1));
    assert(res >= value); // overflow
    return res;
}

template <typename T>
T Pow2AlignDown(T value, uint64_t alignment)
{
    assert(IsPow2(alignment));
    return (value & ~(alignment - 1));
}

uint32_t RoundUpToNextPow2(uint32_t x);
uint64_t RoundUpToNextPow2(uint64_t x);

uint32_t RoundUpToPow2(uint32_t x);
uint64_t RoundUpToPow2(uint64_t x);

template<typename T>
constexpr T RoundUpToMultiple(T value, T alignment)
{
    T res = (((value + (alignment - 1)) / alignment) * alignment);
    assert(res >= value); // overflow
    return res;
}

template <typename T>
constexpr T RoundDownToMultiple(T value, T alignment)
{
    return ((value / alignment) * alignment);
}

} // namespace rad
