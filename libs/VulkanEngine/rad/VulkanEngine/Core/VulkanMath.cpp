#include "VulkanMath.h"

uint32_t vkGetMaxMipLevel(uint32_t width, uint32_t height)
{
    uint32_t maxExtent = std::max(width, height);
    return (uint32_t)std::log2f(float(maxExtent)) + 1;
}

uint32_t vkGetMaxMipLevel(uint32_t width, uint32_t height, uint32_t depth)
{
    uint32_t maxExtent = std::max(std::max(width, height), depth);
    return (uint32_t)std::log2f(float(maxExtent)) + 1;
}

glm::vec3 vkTransformClipToNDC(const glm::vec4& p, float rotation)
{
    float xc = p.x;
    float yc = p.y;
    if (rotation != 0.0f)
    {
        xc = xc * std::cos(rotation) - yc * std::sin(rotation);
        yc = xc * std::sin(rotation) + yc * std::cos(rotation);
    }
    return glm::vec3(xc / p.w, yc / p.w, p.z / p.w);
}

glm::vec3 vkTransformNDCToFragCoord(const glm::vec3& ndc, const VkViewport& viewport,
    bool depthClipNegativeOneToOne)
{
    glm::vec3 fragCoord = {};
    float ox = viewport.x + viewport.width / 2.0f;
    float oy = viewport.y + viewport.height / 2.0f;
    float oz = viewport.minDepth;
    if (depthClipNegativeOneToOne)
    {
        oz = (viewport.maxDepth + viewport.minDepth) / 2.0f;
    }
    float px = viewport.width;
    float py = viewport.height;
    float pz = viewport.maxDepth - viewport.minDepth;
    if (depthClipNegativeOneToOne)
    {
        pz = (viewport.maxDepth - viewport.minDepth) / 2.0f;
    }
    fragCoord.x = (px / 2.0f) * ndc.x + ox;
    fragCoord.y = (py / 2.0f) * ndc.y + oy;
    fragCoord.z = pz * ndc.z + oz;
    return fragCoord;
}

bool vkIsInsideScissor(const glm::vec2& p, const VkRect2D& rect)
{
    return (p.x >= rect.offset.x) && (p.x < rect.offset.x + rect.extent.width) &&
        (p.y >= rect.offset.y) && (p.y < rect.offset.y + rect.extent.height);
}

VulkanBoundingBox Transform(const VulkanBoundingBox& box, const glm::mat4& mat)
{
    VulkanBoundingBox ret;
    for (size_t i = 0; i < 8; ++i)
    {
        glm::vec3 corner = mat * glm::vec4(box.GetCorner(i), 1.0f);
        ret.m_min = glm::min(ret.m_min, corner);
        ret.m_max = glm::max(ret.m_max, corner);
    }
    return ret;
}

static constexpr float MachineEpsilon = std::numeric_limits<float>::epsilon() * 0.5f;

inline constexpr float gamma(size_t n)
{
    return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
}

bool VulkanBoundingBox::IntersectRay(glm::vec3 o, glm::vec3 d, float tMax, float& t0, float& t1) const
{
    t0 = 0;
    t1 = tMax;
    for (int i = 0; i < 3; ++i)
    {
        // Update interval for _i_th bounding box slab
        float invRayDir = 1 / d[i];
        float tNear = (m_min[i] - o[i]) * invRayDir;
        float tFar = (m_max[i] - o[i]) * invRayDir;
        // Update parametric interval from slab intersection $t$ values
        if (tNear > tFar)
        {
            std::swap(tNear, tFar);
        }
        // Update _tFar_ to ensure robust ray--box intersection
        tFar *= 1 + 2 * gamma(3);

        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1)
        {
            return false;
        }
    }
    return true;
}

bool VulkanBoundingBox::IntersectRay(glm::vec3 o, glm::vec3 d, float tMax,
    glm::vec3 invDir, const glm::ivec3 dirSign) const
{
    const VulkanBoundingBox& box = *this;
    // Check for ray intersection against $x$ and $y$ slabs
    float xMin = (box[dirSign[0]].x - o.x) * invDir.x;
    float xMax = (box[1 - dirSign[0]].x - o.x) * invDir.x;
    float yMin = (box[dirSign[1]].y - o.y) * invDir.y;
    float yMax = (box[1 - dirSign[1]].y - o.y) * invDir.y;
    // Update _tMax_ and _tyMax_ to ensure robust box intersection
    xMax *= 1 + 2 * gamma(3);
    yMax *= 1 + 2 * gamma(3);

    if (xMin > yMax || yMin > xMax)
    {
        return false;
    }
    if (yMin > xMin)
    {
        xMin = yMin;
    }
    if (yMax < xMax)
    {
        xMax = yMax;
    }

    // Check for ray intersection against $z$ slab
    float zMin = (box[dirSign[2]].z - o.z) * invDir.z;
    float zMax = (box[1 - dirSign[2]].z - o.z) * invDir.z;
    // Update _tzMax_ to ensure robust box intersection
    zMax *= 1.0f + 2.0f * gamma(3);

    if (xMin > zMax || zMin > xMax)
    {
        return false;
    }
    if (zMin > xMin)
    {
        xMin = zMin;
    }
    if (zMax < xMax)
    {
        xMax = zMax;
    }

    return (xMin < tMax) && (xMax > 0);
}

float VulkanSphere::GetTriangleArea(glm::vec3 a, glm::vec3 b, glm::vec3 c)
{
    return std::abs(2 * std::atan2(glm::dot(a, glm::cross(b, c)),
        1 + glm::dot(a, b) + glm::dot(a, c) + glm::dot(b, c)));
}

float VulkanSphere::GetQuadArea(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d)
{
    glm::vec3 axb = glm::cross(a, b), bxc = glm::cross(b, c);
    glm::vec3 cxd = glm::cross(c, d), dxa = glm::cross(d, a);
    if (rad::LengthSquared(axb) == 0 || rad::LengthSquared(bxc) == 0 || rad::LengthSquared(cxd) == 0 ||
        rad::LengthSquared(dxa) == 0)
    {
        return 0;
    }
    axb = glm::normalize(axb);
    bxc = glm::normalize(bxc);
    cxd = glm::normalize(cxd);
    dxa = glm::normalize(dxa);

    float alpha = rad::AngleBetween(dxa, -axb);
    float beta = rad::AngleBetween(axb, -bxc);
    float gamma = rad::AngleBetween(bxc, -cxd);
    float delta = rad::AngleBetween(cxd, -dxa);

    return std::abs(alpha + beta + gamma + delta - 2 * glm::pi<float>());
}
