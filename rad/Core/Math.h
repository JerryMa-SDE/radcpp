#pragma once

#include "rad/Core/Global.h"
#include <cmath>

namespace rad {

// Finds solutions of the quadratic equation at^2 + bt + c = 0; return true if solutions were found.
bool SolveQuadraticEquation(float a, float b, float c, float& t0, float& t1);

template <typename T>
inline T Sinh(T x)
{
    T u = static_cast<T>(std::expm1(x));
    return T(0.5) * u / (u + T(1.0)) * (u + T(2.0));
}

template <typename T>
inline T SinXDivX(T x)
{
    if (T(1) + x * x == T(1))
    {
        return T(1);
    }
    else
    {
        return std::sin(x) / x;
    }
}

} // namespace rad
