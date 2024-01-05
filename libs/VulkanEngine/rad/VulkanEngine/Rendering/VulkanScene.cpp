#include "VulkanScene.h"
#include "VulkanSceneNode.h"
#include "VulkanCamera.h"
#include "VulkanLight.h"
#include "VulkanMesh.h"
#include "VulkanMaterial.h"

VulkanScene::VulkanScene(rad::Ref<VulkanDevice> device) :
    m_device(std::move(device))
{
    m_root = RAD_NEW VulkanSceneNode(this, nullptr);
}

VulkanScene::~VulkanScene()
{
}
