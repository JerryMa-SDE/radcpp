#pragma once

#include "Global.h"
#include "Integer.h"
#include <compare>

namespace rad
{

// Inspired by vk::Flags: https://github.com/KhronosGroup/Vulkan-Hpp
// Specify Mask type explicitly.
template <typename Mask, typename Bit>
class Flags
{
public:
    constexpr Flags() noexcept : m_mask(0) {}

    template <typename T = std::enable_if_t<!std::is_same_v<T, Bit>, Mask>>
    constexpr explicit Flags(T mask) noexcept : m_mask(mask) {}
    constexpr Flags(Bit bit) noexcept : m_mask(static_cast<Mask>(bit)) {}
    constexpr Flags(Flags<Mask, Bit> const& rhs) noexcept = default;

    Mask GetMask() const
    {
        return m_mask;
    }

    operator Mask() const { return m_mask; }
    Mask* operator&() { return &m_mask; }
    const Mask* operator&() const { return &m_mask; }

    void SetMask(Mask mask)
    {
        m_mask = mask;
    }

    constexpr bool HasBits(Mask bits) noexcept
    {
        return ((m_mask & bits) == bits);
    }

    constexpr bool HasNoBits(Mask bits) noexcept
    {
        return ((m_mask & bits) == 0);
    }

    constexpr bool HasAnyBits(Mask bits) noexcept
    {
        return ((m_mask & bits) != 0);
    }

    int Popcount()
    {
        std::popcount(m_mask);
    }

    // relational operators
#if __cpp_lib_three_way_comparison >= 201907L
    auto operator<=>(Flags<Mask, Bit> const&) const = default;
#else
    constexpr bool operator<(Flags<Mask, Bit> const& rhs) const noexcept
    {
        return m_mask < rhs.m_mask;
    }

    constexpr bool operator<=(Flags<Mask, Bit> const& rhs) const noexcept
    {
        return m_mask <= rhs.m_mask;
    }

    constexpr bool operator>(Flags<Mask, Bit> const& rhs) const noexcept
    {
        return m_mask > rhs.m_mask;
    }

    constexpr bool operator>=(Flags<Mask, Bit> const& rhs) const noexcept
    {
        return m_mask >= rhs.m_mask;
    }

    constexpr bool operator==(Flags<Mask, Bit> const& rhs) const noexcept
    {
        return m_mask == rhs.m_mask;
    }

    constexpr bool operator!=(Flags<Mask, Bit> const& rhs) const noexcept
    {
        return m_mask != rhs.m_mask;
    }
#endif

    // logical operator
    constexpr bool operator!() const noexcept
    {
        return !m_mask;
    }

    // bitwise operators
    constexpr Flags<Mask, Bit> operator&(Flags<Mask, Bit> const& rhs) const noexcept
    {
        return Flags<Mask, Bit>(m_mask & rhs.m_mask);
    }

    constexpr Flags<Mask, Bit> operator|(Flags<Mask, Bit> const& rhs) const noexcept
    {
        return Flags<Mask, Bit>(m_mask | rhs.m_mask);
    }

    constexpr Flags<Mask, Bit> operator^(Flags<Mask, Bit> const& rhs) const noexcept
    {
        return Flags<Mask, Bit>(m_mask ^ rhs.m_mask);
    }

    constexpr Flags<Mask, Bit> operator~() const noexcept
    {
        return Flags<Mask, Bit>(~m_mask);
    }

    // assignment operators
    constexpr Flags<Mask, Bit>& operator=(Flags<Mask, Bit> const& rhs) noexcept = default;

    constexpr Flags<Mask, Bit>& operator|=(Flags<Mask, Bit> const& rhs) noexcept
    {
        m_mask |= rhs.m_mask;
        return *this;
    }

    constexpr Flags<Mask, Bit>& operator&=(Flags<Mask, Bit> const& rhs) noexcept
    {
        m_mask &= rhs.m_mask;
        return *this;
    }

    constexpr Flags<Mask, Bit>& operator^=(Flags<Mask, Bit> const& rhs) noexcept
    {
        m_mask ^= rhs.m_mask;
        return *this;
    }

    // cast operators
    constexpr explicit operator bool() const noexcept
    {
        return !!m_mask;
    }

private:
    Mask m_mask;

}; // class Flags

template<typename Bit>
using Flags32 = Flags<uint32_t, Bit>;

template<typename Bit>
using Flags64 = Flags<uint64_t, Bit>;

#if __cpp_lib_three_way_comparison >= 201907L

// relational operators only needed for pre C++20
template <typename Mask, typename Bit>
constexpr bool operator<(Bit bit, Flags<Mask, Bit> const& flags) noexcept
{
    return flags.operator>(bit);
}

template <typename Mask, typename Bit>
constexpr bool operator<=(Bit bit, Flags<Mask, Bit> const& flags) noexcept
{
    return flags.operator>=(bit);
}

template <typename Mask, typename Bit>
constexpr bool operator>(Bit bit, Flags<Mask, Bit> const& flags) noexcept
{
    return flags.operator<(bit);
}

template <typename Mask, typename Bit>
constexpr bool operator>=(Bit bit, Flags<Mask, Bit> const& flags) noexcept
{
    return flags.operator<=(bit);
}

template <typename Mask, typename Bit>
constexpr bool operator==(Bit bit, Flags<Mask, Bit> const& flags) noexcept
{
    return flags.operator==(bit);
}

template <typename Mask, typename Bit>
constexpr bool operator!=(Bit bit, Flags<Mask, Bit> const& flags) noexcept
{
    return flags.operator!=(bit);
}

#endif // __cpp_lib_three_way_comparison >= 201907L

// bitwise operators
template <typename Mask, typename Bit>
constexpr Flags<Mask, Bit> operator&(Bit bit, Flags<Mask, Bit> const& flags) noexcept
{
    return flags.operator&(bit);
}

template <typename Mask, typename Bit>
constexpr Flags<Mask, Bit> operator|(Bit bit, Flags<Mask, Bit> const& flags) noexcept
{
    return flags.operator|(bit);
}

template <typename Mask, typename Bit>
constexpr Flags<Mask, Bit> operator^(Bit bit, Flags<Mask, Bit> const& flags) noexcept
{
    return flags.operator^(bit);
}

} // namespace rad
