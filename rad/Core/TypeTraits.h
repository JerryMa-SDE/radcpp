#pragma once

#include "Global.h"

RAD_BEGIN_NAMESPACE

template<class T>
constexpr auto UnderlyingCast(T t) noexcept
{
    return static_cast<std::underlying_type_t<T>>(t);
}

RAD_END_NAMESPACE
