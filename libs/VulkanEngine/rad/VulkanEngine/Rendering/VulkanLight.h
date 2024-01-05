#pragma once

#include "rad/VulkanEngine/Core/VulkanMath.h"
#include "rad/Core/String.h"

class VulkanSceneNode;

class VulkanLight : public rad::RefCounted<VulkanLight>
{
public:
    enum class Type
    {
        Directional,
        Point,
        Spot,
        Ambient,
        Rectangle,
        Sphere,
    };

    VulkanLight(VulkanSceneNode* node);
    ~VulkanLight();

    VulkanSceneNode* m_node = nullptr;
    std::string m_name;
    Type m_type = Type::Spot;

    glm::vec3 m_position = { 0, 0, 0 };
    glm::vec3 m_direction = { 0, 0, 0 };
    glm::vec3 m_up = { 0, 1, 0 };

    glm::vec3 m_color = glm::vec3(1.0f, 1.0f, 1.0f);
    float intensity = 1.0f;
    // These two values can be calculated on the CPU and passed into the shader:
    // float lightAngleScale = 1.0f / max(0.001f, cos(innerConeAngle) - cos(outerConeAngle));
    // float lightAngleOffset = -cos(outerConeAngle) * lightAngleScale;
    // Then, in the shader:
    // float cosAngle = dot(spotlightDir, normalizedLightVector);
    // float angularAttenuation = saturate(cosAngle * lightAngleScale + lightAngleOffset);
    // angularAttenuation *= angularAttenuation;
    float m_innerConeAngle = 0.0f;
    float m_outerConeAngle = glm::pi<float>() / 4.0f;

    glm::vec2 m_shape = {};

    // defines a distance cutoff at which the light's intensity must be considered zero.
    // attenuation = max(min(1.0 - (d / range)^4, 1), 0) / d^2;
    float m_range = FLT_MAX;
    // attenuation = 1 / (a0 + a1 * d + a2 * d^2);
    float m_attenuationConstant = 0.0f;     // a0;
    float m_attenuationLinear = 0.0f;       // a1;
    float m_attenuationQuadratic = 0.0f;    // a2;

}; // class VulkanLight
