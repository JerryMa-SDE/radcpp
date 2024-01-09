#pragma once

#include "VulkanCommon.h"
#include "rad/Core/3DMath.h"

uint32_t vkGetMaxMipLevel(uint32_t width, uint32_t height);
uint32_t vkGetMaxMipLevel(uint32_t width, uint32_t height, uint32_t depth);

// @param p vertex position in clip space.
// @param rotation Render Pass Transform, in radians.
glm::vec3 vkTransformClipToNDC(const glm::vec4& p, float rotation = 0.0f);
// @param ndc normalized device coordinate.
glm::vec3 vkTransformNDCToFragCoord(const glm::vec3& ndc, const VkViewport& viewport,
    bool depthClipNegativeOneToOne = false);
bool vkIsInsideScissor(const glm::vec2& p, const VkRect2D& rect);

enum class VulkanDimension
{
    X,
    Y,
    Z,
};

class VulkanSphere
{
public:
    VulkanSphere() = default;
    VulkanSphere(glm::vec3 center, float radius) :
        m_center(center), m_radius(radius) {}
    glm::vec3 m_center = {};
    float m_radius = 0.0f;

    // takes the vertex positions on the unit sphere.
    static float GetTriangleArea(glm::vec3 a, glm::vec3 b, glm::vec3 c);
    // takes the vertex positions on the unit sphere.
    static float GetQuadArea(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 d);
}; // class VulkanSphere

// pbrt::Bounds3 (Axis Aligned): https://github.com/mmp/pbrt-v4/blob/master/src/pbrt/util/vecmath.h
class VulkanBoundingBox
{
public:
    // NOTE: operator[] requires specific memory layout, don't change the order of members!
    glm::vec3 m_min;
    glm::vec3 m_max;

    VulkanBoundingBox()
    {
        constexpr float minValue = std::numeric_limits<float>::lowest();
        constexpr float maxValue = std::numeric_limits<float>::max();
        m_min = glm::vec3(maxValue, maxValue, maxValue);
        m_max = glm::vec3(minValue, minValue, minValue);
    }

    explicit VulkanBoundingBox(glm::vec3 p) :
        m_min(p), m_max(p) {}

    VulkanBoundingBox(glm::vec3 p1, glm::vec3 p2) :
        m_min(glm::min(p1, p2)), m_max(glm::max(p1, p2)) {}

    glm::vec3 operator[](size_t i) const
    {
        assert(i <= 1);
        const glm::vec3* corners = reinterpret_cast<const glm::vec3*>(this);
        return corners[i];
    }

    glm::vec3& operator[](size_t i)
    {
        assert(i <= 1);
        glm::vec3* corners = reinterpret_cast<glm::vec3*>(this);
        return corners[i];
    }

    glm::vec3 GetCorner(size_t i) const
    {
        assert(i < 8);
        return glm::vec3((*this)[(i & 1)].x, (*this)[(i & 2) ? 1 : 0].y,
            (*this)[(i & 4) ? 1 : 0].z);
    }

    glm::vec3 GetDiagonal() const
    {
        return (m_max - m_min);
    }

    float GetSurfaceArea() const
    {
        glm::vec3 d = GetDiagonal();
        return 2 * (d.x * d.y + d.x * d.z + d.y * d.z);
    }

    float GetVolume() const
    {
        glm::vec3 d = GetDiagonal();
        return d.x * d.y * d.z;
    }

    VulkanDimension GetMaxDimension() const
    {
        glm::vec3 d = GetDiagonal();
        if ((d.x > d.y) && (d.x > d.z))
        {
            return VulkanDimension::X;
        }
        else if (d.y > d.z)
        {
            return VulkanDimension::Y;
        }
        else
        {
            return VulkanDimension::Z;
        }
    }

    glm::vec3 Lerp(glm::vec3 t) const
    {
        return glm::vec3(
            glm::mix(m_min.x, m_max.x, t.x),
            glm::mix(m_min.y, m_max.y, t.y),
            glm::mix(m_min.z, m_max.z, t.z));
    }

    glm::vec3 GetNormalized(glm::vec3 p) const
    {
        p = p - m_min;
        if (m_max.x > m_min.x)
        {
            p.x /= m_max.x - m_min.x;
        }
        if (m_max.y > m_min.y)
        {
            p.y /= m_max.y - m_min.y;
        }
        if (m_max.z > m_min.z)
        {
            p.z /= m_max.z - m_min.z;
        }
        return p;
    }

    bool IsInside(glm::vec3 p) const
    {
        return ((p.x >= m_min.x) && (p.x <= m_max.x)) &&
            ((p.y >= m_min.y) && (p.y <= m_max.y)) &&
            ((p.z >= m_min.z) && (p.z <= m_max.z));
    }

    bool IsInsideExclusive(glm::vec3 p) const
    {
        return ((p.x >= m_min.x) && (p.x < m_max.x)) &&
            ((p.y >= m_min.y) && (p.y < m_max.y)) &&
            ((p.z >= m_min.z) && (p.z < m_max.z));
    }

    bool HasOverlap(const VulkanBoundingBox& b)
    {
        bool checkX = (m_max.x >= b.m_min.x) && (m_min.x <= b.m_max.x);
        bool checkY = (m_max.y >= b.m_min.y) && (m_min.y <= b.m_max.y);
        bool checkZ = (m_max.z >= b.m_min.z) && (m_min.z <= b.m_max.z);
        return (checkX && checkY && checkZ);
    }

    VulkanSphere GetBoundingSphere() const
    {
        VulkanSphere s;
        s.m_center = (m_min + m_max) / 2.0f;
        s.m_radius = IsInside(s.m_center) ? glm::distance(s.m_center, m_max) : 0.0f;
        return s;
    }

    bool IsEmpty() const
    {
        return (m_min.x >= m_max.x) || (m_min.y >= m_max.y) || (m_min.z >= m_max.z);
    }

    bool IsDegenerate() const
    {
        return (m_min.x > m_max.x) || (m_min.y > m_max.y) || (m_min.z > m_max.z);
    }

    bool operator==(const VulkanBoundingBox& rhs) const
    {
        return (m_min == rhs.m_min) && (m_max == rhs.m_max);
    }

    bool operator!=(const VulkanBoundingBox& rhs) const
    {
        return (m_min != rhs.m_min) || (m_max != rhs.m_max);
    }

    VulkanBoundingBox Expand(float delta) const
    {
        VulkanBoundingBox b;
        b.m_min = m_min - glm::vec3(delta, delta, delta);
        b.m_max = m_max + glm::vec3(delta, delta, delta);
        return b;
    }

    bool IntersectRay(glm::vec3 o, glm::vec3 d, float tMax, float& t0, float& t1) const;
    bool IntersectRay(glm::vec3 o, glm::vec3 d, float tMax,
        glm::vec3 invDir, const glm::ivec3 dirSign) const;

}; // class VulkanBoundingBox

inline
VulkanBoundingBox Unite(const VulkanBoundingBox& b, const glm::vec3 p)
{
    VulkanBoundingBox ret;
    ret.m_min = glm::min(b.m_min, p);
    ret.m_max = glm::max(b.m_max, p);
    return ret;
}

inline
VulkanBoundingBox Unite(const VulkanBoundingBox& b1, const VulkanBoundingBox& b2)
{
    VulkanBoundingBox ret;
    ret.m_min = glm::min(b1.m_min, b2.m_min);
    ret.m_max = glm::max(b1.m_max, b2.m_max);
    return ret;
}

inline
VulkanBoundingBox Intersect(const VulkanBoundingBox& b1, const VulkanBoundingBox& b2)
{
    VulkanBoundingBox ret;
    ret.m_min = glm::max(b1.m_min, b2.m_min);
    ret.m_max = glm::min(b1.m_max, b2.m_max);
    return ret;
}

inline
float DistanceSquared(glm::vec3 p, const VulkanBoundingBox& b)
{
    float dx = std::max<float>({ 0, b.m_min.x - p.x, p.x - b.m_max.x });
    float dy = std::max<float>({ 0, b.m_min.y - p.y, p.y - b.m_max.y });
    float dz = std::max<float>({ 0, b.m_min.z - p.z, p.z - b.m_max.z });
    return (dx * dx + dy * dy + dz * dz);
}

inline
float Distance(glm::vec3 p, const VulkanBoundingBox& b)
{
    float d2 = DistanceSquared(p, b);
    return glm::sqrt(d2);
}

VulkanBoundingBox Transform(const VulkanBoundingBox& box, const glm::mat4& mat);
