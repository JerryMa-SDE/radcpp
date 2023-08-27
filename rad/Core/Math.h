#pragma once

#include "rad/Core/Global.h"
#include <cmath>

RAD_BEGIN_NAMESPACE

// Finds solutions of the quadratic equation at^2 + bt + c = 0; return true if solutions were found.
bool SolveQuadraticEquation(float a, float b, float c, float& t0, float& t1);

RAD_END_NAMESPACE
