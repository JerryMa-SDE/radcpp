#include "VulkanSceneNode.h"
#include "VulkanScene.h"
#include "VulkanCamera.h"
#include "VulkanLight.h"
#include "VulkanMesh.h"
#include "VulkanMaterial.h"

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
