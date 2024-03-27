#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"

VulkanContext::VulkanContext(rad::Ref<VulkanDevice> device, VulkanQueueFamily family) :
    m_device(std::move(device))
{
    m_queue = m_device->CreateQueue(family);
}

VulkanContext::~VulkanContext()
{
}

rad::Ref<VulkanCommandBuffer>
VulkanContext::AllocateCommandBuffer(VkCommandBufferLevel level)
{
    return m_cmdPool->Allocate(level);
}

rad::Ref<VulkanCommandBuffer>
VulkanContext::AllocateCommandBufferOneTimeUse(VkCommandBufferLevel level)
{
    return m_cmdPoolTransientAlloc->Allocate(level);
}
