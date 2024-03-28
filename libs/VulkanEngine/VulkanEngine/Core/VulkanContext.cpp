#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "VulkanQueue.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"

VulkanContext::VulkanContext(rad::Ref<VulkanDevice> device, VulkanQueueFamily family) :
    m_device(std::move(device))
{
    m_queue = m_device->CreateQueue(family);
}

VulkanContext::~VulkanContext()
{
}

VulkanQueueFamily VulkanContext::GetQueueFamily() const
{
    return m_queue->GetQueueFamily();
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

void VulkanContext::ReadBuffer(
    VulkanBuffer* buffer, void* dest, VkDeviceSize offset, VkDeviceSize size)
{
    if (buffer->IsHostVisible())
    {
        buffer->Read(dest, offset, size);
    }
    else
    {
        rad::Ref<VulkanCommandBuffer> commandBuffer = AllocateCommandBufferOneTimeUse();

        rad::Ref<VulkanBuffer> stagingBuffer = m_device->CreateStagingBuffer(size);
        commandBuffer->Begin();
        VkMemoryBarrier localReadBarrier = {};
        localReadBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        localReadBarrier.pNext;
        localReadBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
        localReadBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = offset;
        copyRegion.dstOffset = 0;
        copyRegion.size = size;
        commandBuffer->SetPipelineBarrier(
            VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            0,
            localReadBarrier, {}, {});
        commandBuffer->CopyBuffer(buffer, stagingBuffer.get(), copyRegion);
        VkBufferMemoryBarrier hostReadBarrier = {};
        hostReadBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        hostReadBarrier.pNext;
        hostReadBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        hostReadBarrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT;
        hostReadBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        hostReadBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        hostReadBarrier.buffer = stagingBuffer->GetHandle();
        hostReadBarrier.offset = 0;
        hostReadBarrier.size = size;
        commandBuffer->SetPipelineBarrier(
            VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_HOST_BIT,
            0,
            {}, hostReadBarrier, {});
        commandBuffer->End();
        m_queue->SubmitAndWaitForCompletion(commandBuffer.get());

        stagingBuffer->Read(dest);
    }
}

void VulkanContext::ReadBuffer(VulkanBuffer* buffer, void* dest)
{
    ReadBuffer(buffer, dest, 0, buffer->GetSize());
}

void VulkanContext::WriteBuffer(
    VulkanBuffer* buffer, const void* data, VkDeviceSize offset, VkDeviceSize size)
{
    assert(offset + size <= buffer->GetSize());
    if (buffer->IsHostVisible())
    {
        buffer->Write(data, offset, size);
    }
    else
    {
        rad::Ref<VulkanBuffer> stagingBuffer = m_device->CreateStagingBuffer(size);
        stagingBuffer->Write(data);

        rad::Ref<VulkanCommandBuffer> commandBuffer = AllocateCommandBufferOneTimeUse();
        commandBuffer->Begin();
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = offset;
        copyRegion.size = size;
        commandBuffer->CopyBuffer(stagingBuffer.get(), buffer, copyRegion);
        commandBuffer->End();
        m_queue->SubmitAndWaitForCompletion(commandBuffer.get());
    }
}

void VulkanContext::WriteBuffer(VulkanBuffer* buffer, const void* data)
{
    WriteBuffer(buffer, data, 0, buffer->GetSize());
}
