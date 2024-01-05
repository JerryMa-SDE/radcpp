#pragma once

#include "rad/VulkanEngine/Core/VulkanMath.h"
#include "rad/Core/String.h"

class VulkanSceneNode;

class VulkanCamera : public rad::RefCounted<VulkanCamera>
{
public:
    VulkanCamera(VulkanSceneNode* node);
    ~VulkanCamera();

    glm::mat4 GetViewMatrix();
    glm::mat4 GetProjectionMatrix();
    glm::mat4 GetViewProjectionMatrix();

    static float ConvertHorizontalFovToVertical(float xfov, float aspectRatio);
    static float ConvertVerticalFovToHorizontal(float yfov, float aspectRatio);

    float GetHorizontalFov();
    float GetVerticalFov();
    void SetHorizontalFov(float xfov);
    void SetVerticalFov(float yfov);

    enum class Type
    {
        Perspective,
        Orthographic,
    };

    VulkanSceneNode* m_node = nullptr;
    std::string m_name;
    Type m_type = Type::Perspective;

    glm::vec3 m_position = glm::vec3(0, 0, 0);
    glm::vec3 m_lookAt = glm::vec3(0, 0, 1);
    glm::vec3 m_up = glm::vec3(0, 1, 0);

    // aspect ratio of the field of view.
    float m_aspectRatio = 0.0f;
    // vertical field of view in radians, should be less than pi.
    float m_yfov = glm::pi<float>() / 2.0f;
    // horizontal magnification.
    float m_xmag = 0.0f;
    // vertical magnification.
    float m_ymag = 0.0f;

    // distance to the near clipping plane.
    float m_clipNear = 0.1f;
    // distance to the far clipping plane.
    float m_clipFar = 1000.0f;

}; // class VulkanCamera
