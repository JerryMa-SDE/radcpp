#include "VulkanCamera.h"

VulkanCamera::VulkanCamera(VulkanSceneNode* node) :
    m_node(node)
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

float VulkanCamera::ConvertHorizontalFovToVertical(float xfov, float aspectRatio)
{
    return 2.0f * std::atan(std::tan(xfov / 2.0f) / aspectRatio);
}

float VulkanCamera::ConvertVerticalFovToHorizontal(float yfov, float aspectRatio)
{
    return 2.0f * std::atan(std::tan(yfov / 2.0f) * aspectRatio);
}

float VulkanCamera::GetHorizontalFov()
{
    return ConvertVerticalFovToHorizontal(m_yfov, m_aspectRatio);
}

float VulkanCamera::GetVerticalFov()
{
    return m_yfov;
}

void VulkanCamera::SetHorizontalFov(float xfov)
{
    m_yfov = ConvertHorizontalFovToVertical(xfov, m_aspectRatio);
}

void VulkanCamera::SetVerticalFov(float yfov)
{
    m_yfov = yfov;
}
