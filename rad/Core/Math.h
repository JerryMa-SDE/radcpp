#pragma once

#include "rad/Core/Global.h"
#include <cmath>
#include <numbers>
// e = e_v<double>;
// log2e = log2e_v<double>;
// log10e = log10e_v<double>;
// pi = pi_v<double>;
// inv_pi = inv_pi_v<double>; // 1/pi
// inv_sqrtpi = inv_sqrtpi_v<double>; // 1/sqrt(pi)
// ln2 = ln2_v<double>;
// ln10 = ln10_v<double>;
// sqrt2 = sqrt2_v<double>;
// sqrt3 = sqrt3_v<double>;
// inv_sqrt3 = inv_sqrt3_v<double>; // 1/sqrt(3)
// egamma = egamma_v<double>; // the Euler-Mascheroni constant: https://en.wikipedia.org/wiki/Euler%27s_constant
// phi = phi_v<double>; // the golden ratio: (1+sqrt(5))/2 = 1.618033988749...

namespace rad
{

// Finds solutions of the quadratic equation at^2 + bt + c = 0; return true if solutions were found.
bool SolveQuadraticEquation(float a, float b, float c, float& t0, float& t1);

// The Right Way to Calculate Stuff: https://www.plunk.org/~hatch/rightway.html
// 1-cos(x) = 2*sin(x/2)^2 // OneMinusCosx
// acos(1-x) = 2*asin(sqrt(x/2))
// e^x-1 = expm1(x)
// ln(1+x) = log1p(x)
// tanh(x) = expm1(2*x) / (expm1(2*x) + 2) // overflows prematurely
// atanh(x) = log1p(2*x/(1-x)) / 2
// sinh(x) = expm1(x) * (expm1(x)+2) / (expm1(x)+1) / 2
// asinh(x) = log1p(x * (1 + x/(sqrt(x2+1)+1)))
// cosh(x)-1
// acos(1+x)
// sqrt(1+x)-1 = x / (sqrt(1+x)+1)
// 1-sqrt(1-x) = x / (sqrt(1-x)+1)
// (1+x)^2-1 = x * (2 + x)
// 1-(1-x)^2 = x * (2 - x)
// sin(x)/x // Sinx_Div_x
// (1-cos(x))/x // OneMinusCosx_Div_x

// 1-cos(x) = 2*sin(x/2)^2
double OneMinusCosx(double x);
// sin(x)/x, if x != 0; 1 if x = 0.
double Sinx_Div_x(double x);
// (1-cos(x))/x
double OneMinusCosx_Div_x(double x);

} // namespace rad
