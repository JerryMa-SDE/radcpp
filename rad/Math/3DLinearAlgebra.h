#pragma once

#include "rad/Core/Global.h"
#include "Math.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"

namespace rad
{
inline float LengthSquared(glm::vec3 v)
{
    return (v.x * v.x + v.y * v.y + v.z * v.z);
}

// returns angle between two unit vectors in radians, equivalent to `acos(dot(a, b))`,
// but more numerically stable.
float AngleBetween(glm::vec3 v1, glm::vec3 v2);

} // namespace rad
