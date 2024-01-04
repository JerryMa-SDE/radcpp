#include "VulkanCamera.h"

VulkanCamera::VulkanCamera()
{
}

VulkanCamera::~VulkanCamera()
{
}

glm::mat4 VulkanCamera::GetViewMatrix()
{
    return glm::lookAt(m_position, m_lookAt, m_up);
}

glm::mat4 VulkanCamera::GetProjectionMatrix()
{
    // Vulkan clip space has inverted Y and half Z.
    const glm::mat4 clip(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.5f, 0.0f,
        0.0f, 0.0f, 0.5f, 1.0f);
    if (m_type == Type::Perspective)
    {
        return clip * glm::perspective(m_yfov, m_aspectRatio, m_clipNear, m_clipFar);
    }
    else
    {
        // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#orthographic-projection
        float r = m_xmag / 2.0f;
        float t = m_ymag / 2.0f;
        float f = m_clipFar;
        float n = m_clipNear;
        return clip * glm::mat4(
            1.0f / r, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f / t, 0.0f, 0.0f,
            0.0f, 0.0f, 2.0f / (n - f), 0.0f,
            0.0f, 0.0f, (f + n) / (n - f), 1.0f
        );
    }
}

glm::mat4 VulkanCamera::GetViewProjectionMatrix()
{
    return GetProjectionMatrix() * GetViewMatrix();
}
