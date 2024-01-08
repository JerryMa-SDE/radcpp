#include "VulkanSceneNode.h"
#include "VulkanScene.h"
#include "VulkanCamera.h"
#include "VulkanLight.h"
#include "VulkanMesh.h"
#include "VulkanMaterial.h"

#include "assimp/scene.h"
#include "assimp/cimport.h"
#include "assimp/postprocess.h"

VulkanSceneNode::VulkanSceneNode(VulkanScene* scene, VulkanSceneNode* parent) :
    m_scene(scene),
    m_parent(parent)
{
}

VulkanSceneNode::~VulkanSceneNode()
{
}

VulkanDevice* VulkanSceneNode::GetDevice()
{
    return m_scene->m_device.get();
}

VulkanSceneNode* VulkanSceneNode::AddChild(std::string_view name)
{
    auto& child = m_children.emplace_back(RAD_NEW VulkanSceneNode(m_scene, this));
    child->m_name = name;
    return child.get();
}

bool VulkanSceneNode::ImportFromFile(const rad::FilePath& filePath)
{
    const struct aiScene* sceneData = nullptr;
    if (filePath.has_filename() && rad::Exists(filePath))
    {
        sceneData = aiImportFile((const char*)filePath.u8string().c_str(),
            aiProcessPreset_TargetRealtime_Fast);
    }
    else
    {
        LogVulkan(Error, "VulkanSceneNode::ImportFromFile failed: %s (not exist)",
            (const char*)filePath.u8string().c_str());
        return false;
    }

    if (sceneData)
    {
        m_filePath = rad::MakeAbsolute(filePath);
        m_baseDir = m_filePath.parent_path();

        aiReleaseImport(sceneData);
        return true;
    }
    else
    {
        LogVulkan(Error, "VulkanSceneNode::ImportFromFile failed: %s (%s)",
            (const char*)filePath.u8string().c_str(), aiGetErrorString());
        return false;
    }

    return false;
}
