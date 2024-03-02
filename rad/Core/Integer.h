#pragma once

#include "Global.h"
#include <cassert>

#include <bit>
// check endianness of scalar types: std::endian::native == std::endian::[little,big]
// bit_cast: reinterpret the object representation of one type as that of another
// byteswap: reverses the bytes in the given integer value (C++23)
// has_single_bit: checks if a number is an integral power of 2
// bit_ceil: finds the smallest integral power of two not less than the given value
// bit_floor: finds the largest integral power of two not greater than the given value
// bit_width: finds the smallest number of bits needed to represent the given value
// rotl: computes the result of bitwise left-rotation
// rotr: computes the result of bitwise right-rotation
// countl_zero: counts the number of consecutive ​0​ bits, starting from the most significant bit
// countl_one: counts the number of consecutive 1 bits, starting from the most significant bit
// countr_zero: counts the number of consecutive ​0​ bits, starting from the least significant bit
// countr_one: counts the number of consecutive 1 bits, starting from the least significant bit
// popcount: counts the number of 1 bits in an unsigned integer

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

/// Count the number of bits set in an unsigned integer (popcount).
uint32_t CountBits(uint32_t x);
uint64_t CountBits(uint64_t x);

/// Search the mask data from most significant bit (MSB) to least significant bit (LSB) for a set bit (1).
/// The mask must be nonzero or the index returned is undefined.
uint32_t BitScanReverse32(uint32_t mask);
uint32_t BitScanReverse64(uint64_t mask);

constexpr bool IsPow2(uint64_t x)
{
    //return (x == 0) ? false : ((x & (x - 1)) == 0);
    return x && !(x & (x - 1));
}

template <typename T>
T Pow2AlignUp(T value, T alignment)
{
    assert(IsPow2(alignment));
    assert(value + static_cast<T>(alignment) > value);
    return ((value + static_cast<T>(alignment) - 1) & ~(static_cast<T>(alignment) - 1));
}

template <typename T>
T Pow2AlignDown(T value, T alignment)
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
    if (value != 0)
    {
        assert((value + (alignment - 1)) >= value);
        return (((value + (alignment - 1)) / alignment) * alignment);
    }
    else
    {
        return 0;
    }
}

template <typename T>
constexpr T RoundDownToMultiple(T value, T alignment)
{
    return ((value / alignment) * alignment);
}

} // namespace rad
