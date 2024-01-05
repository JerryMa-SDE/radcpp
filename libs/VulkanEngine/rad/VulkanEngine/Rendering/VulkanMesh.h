#pragma once

#include "rad/VulkanEngine/Core.h"

class VulkanSceneNode;

class VulkanMesh : public rad::RefCounted<VulkanMesh>
{
public:
    VulkanMesh(VulkanSceneNode* node);
    ~VulkanMesh();

    VulkanSceneNode* m_node = nullptr;
    std::string m_name;
    enum class PrimtiveType
    {
        Point = 0x1,
        Line = 0x2,
        Triangle = 0x4,
        Polygon = 0x8,
    };
    rad::Flags32<PrimtiveType> m_primTypes;
    std::vector<glm::vec3> m_positions;
    std::vector<glm::vec3> m_normals;
    std::vector<glm::vec3> m_tangents;
    std::vector<glm::vec3> m_bitangents;
    std::vector<std::vector<glm::vec4>> m_colorChannels;
    std::vector<std::vector<glm::vec2>> m_uvChannels;
    std::vector<std::string> m_uvNames;

    struct Polygon
    {
        std::vector<uint32_t> indices;
    };

    std::vector<Polygon> m_polygons;

    struct VertexWeight
    {
        uint32_t index;
        float weight;
    };

    struct Bone
    {
        std::string name;
        std::vector<VertexWeight> weights;
        glm::mat4 inverseBindMatrix;
    };

    std::vector<Bone> m_bones;

    enum class MorphingMethod
    {
        VertexBlend,
        Normalized,
        Relative,
    };
    MorphingMethod m_morphingMethod = MorphingMethod::VertexBlend;

}; // class VulkanMesh
