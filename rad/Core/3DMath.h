#pragma once

#include "Global.h"
#include "Math.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"

namespace rad {

inline float LengthSquared(glm::vec3 v)
{
    return (v.x * v.x + v.y * v.y + v.z * v.z);
}

// returns angle between unit vectors in radians, equivalent to `acos(dot(a, b))`, but more numerically stable.
// http://www.plunk.org/~hatch/rightway.html
inline float AngleBetween(glm::vec3 v1, glm::vec3 v2)
{
    if (glm::dot(v1, v2) < 0)
    {
        return glm::pi<float>() - 2 * glm::asin(glm::length(v1 + v2) / 2);
    }
    else
    {
        return 2 * glm::asin(glm::length(v2 - v1) / 2);
    }
}

} // namespace rad
