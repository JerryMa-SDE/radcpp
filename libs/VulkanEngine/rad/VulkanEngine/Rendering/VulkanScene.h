#pragma once

#include "rad/VulkanEngine/Core.h"
#include "VulkanSceneNode.h"

class VulkanSceneNode;
class VulkanCamera;
class VulkanLight;
class VulkanMesh;
class VulkanMaterial;

class VulkanScene : rad::RefCounted<VulkanScene>
{
public:
    VulkanScene(rad::Ref<VulkanDevice> device);
    ~VulkanScene();

    rad::Ref<VulkanDevice> m_device;
    rad::Ref<VulkanSceneNode> m_root;

    VulkanCamera* m_camera = nullptr;
    std::vector<VulkanLight*> m_lights;

}; // class VulkanScene
