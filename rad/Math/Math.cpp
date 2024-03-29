#include "Math.h"

namespace rad
{

// https://pbr-book.org/3ed-2018/Utilities/Mathematical_Routines
bool SolveQuadraticEquation(float a, float b, float c, float& t0, float& t1)
{
    // b^2 - 4ac
    double discrim = (double)b * (double)b - 4 * (double)a * (double)c;
    if (discrim < 0)
    {
        return false;
    }
    double rootDiscrim = std::sqrt(discrim);
    double q = 0.0;
    if (b < 0)
    {
        q = -.5 * (b - rootDiscrim);
    }
    else
    {
        q = -.5 * (b + rootDiscrim);
    }
    t0 = static_cast<float>(q / a);
    t1 = static_cast<float>(c / q);
    if (t0 > t1)
    {
        std::swap(t0, t1);
    }
    return true;
}

// https://www.plunk.org/~hatch/rightway.html
double OneMinusCosx(double x)
{
    return 2.0 * pow(sin(x / 2.0), 2);
}

// https://www.plunk.org/~hatch/rightway.html
double Sinx_Div_x(double x)
{
    if (1.0 + x * x == 1.0)
    {
        return 1.0;
    }
    else
    {
        return sin(x) / x;
    }
}

// https://www.plunk.org/~hatch/rightway.html
double OneMinusCosx_Div_x(double x)
{
    if (1.0 + x * x == 1.0)
    {
        return 0.5 * x;
    }
    else
    {
        return OneMinusCosx(x) / x;
    }
}

namespace internal
{

// https://www.plunk.org/~hatch/rightway.html
double expm1(double x)
{
    double u = exp(x);
    if (u == 1.0)
    {
        return x;
    }
    if (u - 1.0 == -1.0)
    {
        return -1.0;
    }
    return (u - 1.0) * x / log(u);  // where log is natural logarithm
}

// https://www.plunk.org/~hatch/rightway.html
double log1p(double x)
{
    double u = 1.0 + x;
    if (u == 1.0)
    {
        return x;
    }
    else
    {
        return log(u) * x / (u - 1.0);
    }
}

// https://www.plunk.org/~hatch/rightway.html
double sinh(double x)
{
    double u = std::expm1(x);
    return 0.5 * u / (u + 1.0) * (u + 2.0);
}

} // namespace internal

} // namespace rad
