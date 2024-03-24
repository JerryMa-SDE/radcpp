#include "3DLinearAlgebra.h"

namespace rad
{

// http://www.plunk.org/~hatch/rightway.html
float AngleBetween(glm::vec3 v1, glm::vec3 v2)
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
