#pragma once

#include "Global.h"
#include <concepts>

namespace rad
{

template<class T>
constexpr auto UnderlyingCast(T t) noexcept
{
    return static_cast<std::underlying_type_t<T>>(t);
}

template <typename...>
struct MaxSizeof;

template <>
struct MaxSizeof<>
{
    static constexpr uint32_t value = 0;
};

template <typename T, typename... Rest>
struct MaxSizeof<T, Rest...>
{
    static constexpr uint32_t value = sizeof(T) > MaxSizeof<Rest...>::value ?
        sizeof(T) : MaxSizeof<Rest...>::value;
};

template <class T>
concept Enumeration = std::is_enum_v<T>;

} // namespace rad
