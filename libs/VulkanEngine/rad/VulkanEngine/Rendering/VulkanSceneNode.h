#pragma once

#include "rad/VulkanEngine/Core.h"
#include <map>

class VulkanScene;
class VulkanCamera;
class VulkanLight;
class VulkanMesh;
class VulkanMaterial;

class VulkanSceneNode : public rad::RefCounted<VulkanSceneNode>
{
public:
    VulkanSceneNode(VulkanScene* scene, VulkanSceneNode* parent);
    ~VulkanSceneNode();

    VulkanDevice* GetDevice();
    VulkanSceneNode* AddChild(std::string_view name = "");

    bool ImportFromFile(const rad::FilePath& filePath);

    VulkanScene* m_scene = nullptr;
    VulkanSceneNode* m_parent = nullptr;
    std::vector<rad::Ref<VulkanSceneNode>> m_children;

    std::string m_name;
    rad::FilePath m_filePath;
    rad::FilePath m_baseDir;

    std::vector<rad::Ref<VulkanMesh>> m_meshes;
    std::vector<rad::Ref<VulkanMaterial>> m_materials;
    std::vector<rad::Ref<VulkanCamera>> m_cameras;
    std::vector<rad::Ref<VulkanLight>> m_lights;

    std::map<rad::FilePath, rad::Ref<VulkanImage>> m_images;

}; // class VulkanSceneNode
