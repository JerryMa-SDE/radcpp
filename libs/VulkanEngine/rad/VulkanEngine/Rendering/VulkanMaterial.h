#pragma once

#include "rad/VulkanEngine/Core.h"
#include "VulkanTexture.h"

class VulkanSceneNode;

class VulkanMaterial : public rad::RefCounted<VulkanMaterial>
{
public:
    VulkanMaterial(VulkanSceneNode* node);
    ~VulkanMaterial();

    VulkanSceneNode* m_node = nullptr;

    struct PbrMetallicRoughness
    {
        glm::vec4 baseColor;
        float metallic;
        float roughness;
    };

    struct TextureInfo : public rad::RefCounted<TextureInfo>
    {
        VulkanImage* image;
        VulkanTextureMapping mapping;
        uint32_t uvIndex;
        VulkanTextureBlendOp blendOp;
        float blend;
        VkSamplerAddressMode addressMode[3]; // uvw
    };

    float m_normalScale = 1.0f;

    glm::vec4 m_baseColor = { 1, 1, 1, 1 };
    float m_metallic = 1.0f;
    float m_roughness = 1.0f;
    glm::vec3 m_emissiveColor = {};
    glm::vec3 m_embientColor = {};
    float m_embientStrength = 1.0f;
    enum AlphaMode
    {
        Opaque,
        Mask,
        Blend,
    };
    float m_alphaCutoff = 0.5f;
    bool m_isDoubleSided = false;

    rad::Ref<TextureInfo> m_displacementTexture;
    rad::Ref<TextureInfo> m_normalTexture;
    rad::Ref<TextureInfo> m_baseColorTexture;
    rad::Ref<TextureInfo> m_metallicRoughnessTexture;
    rad::Ref<TextureInfo> m_ambientTexture;
    rad::Ref<TextureInfo> m_emissiveTexture;
    rad::Ref<TextureInfo> m_transmissionTexture;
    rad::Ref<TextureInfo> m_iridescenceTexture;
    rad::Ref<TextureInfo> m_sheenTexture;
    rad::Ref<TextureInfo> m_clearcoatTexture;

}; // class VulkanMaterial
