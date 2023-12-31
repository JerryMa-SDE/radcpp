#include "VulkanCommandBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanCommandPool.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSet.h"
#include "VulkanDevice.h"
#include "VulkanEvent.h"
#include "VulkanFence.h"
#include "VulkanFormat.h"
#include "VulkanFramebuffer.h"
#include "VulkanImage.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanPipeline.h"
#include "VulkanQueue.h"
#include "VulkanRenderPass.h"
#include "VulkanSampler.h"
#include "VulkanSemaphore.h"
#include "VulkanShader.h"
#include "VulkanSwapchain.h"

#include "boost/container/small_vector.hpp"

VulkanCommandBuffer::VulkanCommandBuffer(rad::Ref<VulkanDevice> device, rad::Ref<VulkanCommandPool> commandPool,
    VkCommandBufferLevel level) :
    m_device(std::move(device)),
    m_commandPool(std::move(commandPool)),
    m_level(level)
{
    VkCommandBufferAllocateInfo allocateInfo = {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.commandPool = m_commandPool->GetHandle();
    allocateInfo.level = level;
    allocateInfo.commandBufferCount = 1;
    VK_CHECK(m_device->GetFunctionTable()->
        vkAllocateCommandBuffers(m_device->GetHandle(), &allocateInfo, &m_handle));
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
    m_device->GetFunctionTable()->
        vkFreeCommandBuffers(m_device->GetHandle(), m_commandPool->GetHandle(), 1, &m_handle);
}

void VulkanCommandBuffer::Reset(VkCommandBufferResetFlags flags)
{
    VK_CHECK(m_device->GetFunctionTable()->
        vkResetCommandBuffer(m_handle, flags));
}

void VulkanCommandBuffer::Begin(
    VkCommandBufferUsageFlags flags,
    const VkCommandBufferInheritanceInfo* pInheritanceInfo)
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.flags = flags;
    beginInfo.pInheritanceInfo = pInheritanceInfo;

    VK_CHECK(m_device->GetFunctionTable()->
        vkBeginCommandBuffer(m_handle, &beginInfo));
}

void VulkanCommandBuffer::End()
{
    VK_CHECK(m_device->GetFunctionTable()->
        vkEndCommandBuffer(m_handle));
}

void VulkanCommandBuffer::BeginRenderPass(
    const VkRenderPassBeginInfo& beginInfo,
    VkSubpassContents contents)
{
    m_device->GetFunctionTable()->vkCmdBeginRenderPass(m_handle, &beginInfo, contents);
}

void VulkanCommandBuffer::BeginRenderPass(
    VulkanRenderPass* renderPass, VulkanFramebuffer* framebuffer,
    const VkRect2D& renderArea, rad::Span<VkClearValue> clearValues,
    VkSubpassContents contents)
{
    VkRenderPassBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.renderPass = renderPass->GetHandle();
    beginInfo.framebuffer = framebuffer->GetHandle();
    beginInfo.renderArea = renderArea;
    beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    beginInfo.pClearValues = clearValues.data();

    BeginRenderPass(beginInfo, contents);
}

void VulkanCommandBuffer::BeginRenderPass(
    VulkanRenderPass* renderPass, VulkanFramebuffer* framebuffer,
    rad::Span<VkClearValue> clearValues)
{
    VkRenderPassBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    beginInfo.pNext = nullptr;
    beginInfo.renderPass = renderPass->GetHandle();
    beginInfo.framebuffer = framebuffer->GetHandle();
    beginInfo.renderArea.offset.x = 0;
    beginInfo.renderArea.offset.y = 0;
    beginInfo.renderArea.extent.width = framebuffer->GetWidth();
    beginInfo.renderArea.extent.height = framebuffer->GetHeight();
    beginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    beginInfo.pClearValues = clearValues.data();

    BeginRenderPass(beginInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void VulkanCommandBuffer::EndRenderPass()
{
    m_device->GetFunctionTable()->vkCmdEndRenderPass(m_handle);
}

void VulkanCommandBuffer::NextSubpass(VkSubpassContents contents)
{
    m_device->GetFunctionTable()->
        vkCmdNextSubpass(m_handle, contents);
}

void VulkanCommandBuffer::BeginRendering(const VkRenderingInfoKHR& renderingInfo)
{
    vkCmdBeginRenderingKHR(m_handle, &renderingInfo);
}

void VulkanCommandBuffer::BeginRendering(rad::Span<VulkanImageView*> colorViews, VulkanImageView* depthStencilView,
    bool loadClear, const VkClearColorValue& clearColor, const VkClearDepthStencilValue& clearDepthStencil)
{
    std::vector<VkRenderingAttachmentInfoKHR> colorInfos = {};
    colorInfos.reserve(colorViews.size());
    for (auto colorView : colorViews)
    {
        VkRenderingAttachmentInfoKHR colorInfo = {};
        colorInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        colorInfo.imageView = colorView->GetHandle();
        colorInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        if (loadClear)
        {
            colorInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        }
        else
        {
            colorInfo.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }
        colorInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorInfo.clearValue.color = clearColor;
        colorInfos.push_back(colorInfo);
    }

    VkRenderingAttachmentInfoKHR depthStencilInfo = {};
    if (depthStencilView)
    {
        depthStencilInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        depthStencilInfo.imageView = depthStencilView->GetHandle();
        depthStencilInfo.imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        if (loadClear)
        {
            depthStencilInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        }
        else
        {
            depthStencilInfo.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        }
        depthStencilInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depthStencilInfo.clearValue.depthStencil = clearDepthStencil;
    }

    uint32_t width = 0;
    uint32_t height = 0;
    if (!colorViews.empty())
    {
        width = colorViews[0]->GetImage()->GetWidth();
        height = colorViews[0]->GetImage()->GetHeight();
    }
    else if (depthStencilView)
    {
        width = depthStencilView->GetImage()->GetWidth();
        height = depthStencilView->GetImage()->GetHeight();
    }

    VkRenderingInfoKHR renderingInfo = {};
    renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    renderingInfo.renderArea = { 0, 0, width, height };
    renderingInfo.layerCount = 1;
    renderingInfo.colorAttachmentCount = 1;
    renderingInfo.pColorAttachments = colorInfos.data();
    if (depthStencilView)
    {
        renderingInfo.pDepthAttachment = &depthStencilInfo;
        renderingInfo.pStencilAttachment = &depthStencilInfo;
    }

    BeginRendering(renderingInfo);
}

void VulkanCommandBuffer::EndRendering()
{
    vkCmdEndRenderingKHR(m_handle);
}

void VulkanCommandBuffer::BindPipeline(VulkanPipeline* pipeline)
{
    m_device->GetFunctionTable()->
        vkCmdBindPipeline(m_handle, pipeline->GetBindPoint(), pipeline->GetHandle());
}

void VulkanCommandBuffer::BindDescriptorSets(
    VulkanPipeline* pipeline,
    VulkanPipelineLayout* layout,
    uint32_t                            firstSet,
    rad::Span<VulkanDescriptorSet*>      descSets,
    rad::Span<uint32_t>                  dynamicOffsets)
{
    boost::container::small_vector<VkDescriptorSet, 8> descSetsHandles(descSets.size());
    for (size_t i = 0; i < descSets.size(); i++)
    {
        descSetsHandles[i] = descSets[i]->GetHandle();
    }

    m_device->GetFunctionTable()->
        vkCmdBindDescriptorSets(m_handle, pipeline->GetBindPoint(), layout->GetHandle(),
            firstSet, static_cast<uint32_t>(descSetsHandles.size()), descSetsHandles.data(),
            static_cast<uint32_t>(dynamicOffsets.size()), dynamicOffsets.data());
}

void VulkanCommandBuffer::SetScissors(rad::Span<VkRect2D> scissors, uint32_t first)
{
    m_device->GetFunctionTable()->
        vkCmdSetScissor(m_handle, first, static_cast<uint32_t>(scissors.size()), scissors.data());
}

void VulkanCommandBuffer::SetViewports(rad::Span<VkViewport> viewports, uint32_t first)
{
    m_device->GetFunctionTable()->
        vkCmdSetViewport(m_handle, first, static_cast<uint32_t>(viewports.size()), viewports.data());
}

void VulkanCommandBuffer::SetDepthBounds(float min, float max)
{
    m_device->GetFunctionTable()->
        vkCmdSetDepthBounds(m_handle, min, max);
}

void VulkanCommandBuffer::SetDepthStencilCompareMask(VkStencilFaceFlags faceMask, uint32_t compareMask)
{
    m_device->GetFunctionTable()->
        vkCmdSetStencilCompareMask(m_handle, faceMask, compareMask);
}

void VulkanCommandBuffer::SetStencilWriteMask(VkStencilFaceFlags faceMask, uint32_t writeMask)
{
    m_device->GetFunctionTable()->
        vkCmdSetStencilWriteMask(m_handle, faceMask, writeMask);
}

void VulkanCommandBuffer::SetStencilReference(VkStencilFaceFlags faceMask, uint32_t reference)
{
    m_device->GetFunctionTable()->
        vkCmdSetStencilReference(m_handle, faceMask, reference);
}

void VulkanCommandBuffer::SetBlendConstants(float r, float g, float b, float a)
{
    float blendConstants[4] = { r, g, b, a };
    m_device->GetFunctionTable()->
        vkCmdSetBlendConstants(m_handle, blendConstants);
}

void VulkanCommandBuffer::SetLineWidth(float width)
{
    m_device->GetFunctionTable()->
        vkCmdSetLineWidth(m_handle, width);
}

void VulkanCommandBuffer::SetDepthBias(float constantFactor, float clamp, float slopeFactor)
{
    m_device->GetFunctionTable()->
        vkCmdSetDepthBias(m_handle, constantFactor, clamp, slopeFactor);
}

void VulkanCommandBuffer::BindIndexBuffer(VulkanBuffer* buffer, VkDeviceSize offset, VkIndexType indexType)
{
    m_device->GetFunctionTable()->
        vkCmdBindIndexBuffer(m_handle, buffer->GetHandle(), offset, indexType);
}

void VulkanCommandBuffer::BindVertexBuffers(
    uint32_t firstBinding, rad::Span<VulkanBuffer*> buffers, rad::Span<VkDeviceSize> offsets)
{
    boost::container::small_vector<VkBuffer, 8> buffersHandles(buffers.size());
    for (size_t i = 0; i < buffersHandles.size(); i++)
    {
        buffersHandles[i] = buffers[i]->GetHandle();
    }

    m_device->GetFunctionTable()->vkCmdBindVertexBuffers(m_handle,
        firstBinding, static_cast<uint32_t>(buffersHandles.size()), buffersHandles.data(), offsets.data());
}

void VulkanCommandBuffer::BindVertexBuffers(uint32_t firstBinding, rad::Span<VulkanBuffer*> buffers)
{
    boost::container::small_vector<VkDeviceSize, 8> offsets(buffers.size(), 0);
    return BindVertexBuffers(firstBinding, buffers, { offsets.data(), offsets.size() });
}

void VulkanCommandBuffer::Draw(
    uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
{
    m_device->GetFunctionTable()->vkCmdDraw(m_handle, vertexCount, instanceCount, firstVertex, firstInstance);
}

void VulkanCommandBuffer::DrawIndexed(
    uint32_t indexCount, uint32_t instanceCount,
    uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance)
{
    m_device->GetFunctionTable()->
        vkCmdDrawIndexed(m_handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}

void VulkanCommandBuffer::DrawIndirect(
    VulkanBuffer* buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
    m_device->GetFunctionTable()->
        vkCmdDrawIndirect(m_handle, buffer->GetHandle(), offset, drawCount, stride);
}

void VulkanCommandBuffer::DrawIndexedIndirect(
    VulkanBuffer* buffer, VkDeviceSize offset, uint32_t drawCount, uint32_t stride)
{
    m_device->GetFunctionTable()->
        vkCmdDrawIndexedIndirect(m_handle, buffer->GetHandle(), offset, drawCount, stride);
}

void VulkanCommandBuffer::Dispatch(uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    m_device->GetFunctionTable()->
        vkCmdDispatch(m_handle, groupCountX, groupCountY, groupCountZ);
}

void VulkanCommandBuffer::DispatchIndirect(VulkanBuffer* buffer, VkDeviceSize offset)
{
    m_device->GetFunctionTable()->vkCmdDispatchIndirect(m_handle, buffer->GetHandle(), offset);
}

void VulkanCommandBuffer::DispatchBase(
    uint32_t baseGroupX, uint32_t baseGroupY, uint32_t baseGroupZ,
    uint32_t groupCountX, uint32_t groupCountY, uint32_t groupCountZ)
{
    m_device->GetFunctionTable()->vkCmdDispatchBase(m_handle,
        baseGroupX, baseGroupY, baseGroupZ,
        groupCountX, groupCountY, groupCountZ);
}

void VulkanCommandBuffer::ClearColorImage(
    VulkanImage* image, VkImageLayout layout,
    const VkClearColorValue& color, rad::Span<VkImageSubresourceRange> ranges)
{
    m_device->GetFunctionTable()->
        vkCmdClearColorImage(m_handle, image->GetHandle(), layout, &color,
            static_cast<uint32_t>(ranges.size()), ranges.data());
}

void VulkanCommandBuffer::ClearDepthStencilImage(
    VulkanImage* image, VkImageLayout layout,
    const VkClearDepthStencilValue& value, rad::Span<VkImageSubresourceRange> ranges)
{
    m_device->GetFunctionTable()->
        vkCmdClearDepthStencilImage(
            m_handle, image->GetHandle(), layout,
            &value, static_cast<uint32_t>(ranges.size()), ranges.data());
}

void VulkanCommandBuffer::ClearAttachments(rad::Span<VkClearAttachment> attachments, rad::Span<VkClearRect> rects)
{
    m_device->GetFunctionTable()->
        vkCmdClearAttachments(m_handle,
            static_cast<uint32_t>(attachments.size()), attachments.data(),
            static_cast<uint32_t>(rects.size()), rects.data());
}

void VulkanCommandBuffer::FillBuffer(
    VulkanBuffer* dstBuffer, VkDeviceSize dstOffset, VkDeviceSize size, uint32_t data)
{
    m_device->GetFunctionTable()->
        vkCmdFillBuffer(m_handle, dstBuffer->GetHandle(), dstOffset, size, data);
}

void VulkanCommandBuffer::UpdateBuffer(VulkanBuffer* dstBuffer,
    VkDeviceSize offset, VkDeviceSize dataSize, const uint32_t* pData)
{
    m_device->GetFunctionTable()->
        vkCmdUpdateBuffer(m_handle, dstBuffer->GetHandle(), offset, dataSize, pData);
}

void VulkanCommandBuffer::CopyBuffer(
    VulkanBuffer* srcBuffer, VulkanBuffer* dstBuffer, rad::Span<VkBufferCopy> regions)
{
    m_device->GetFunctionTable()->
        vkCmdCopyBuffer(m_handle, srcBuffer->GetHandle(), dstBuffer->GetHandle(),
            static_cast<uint32_t>(regions.size()), regions.data());
}

void VulkanCommandBuffer::CopyImage(
    VulkanImage* srcImage, VkImageLayout srcImageLayout,
    VulkanImage* dstImage, VkImageLayout dstImageLayout,
    rad::Span<VkImageCopy> regions)
{
    m_device->GetFunctionTable()->
        vkCmdCopyImage(
            m_handle, srcImage->GetHandle(), srcImageLayout,
            dstImage->GetHandle(), dstImageLayout,
            static_cast<uint32_t>(regions.size()), regions.data()
        );
}

void VulkanCommandBuffer::CopyBufferToImage(
    VulkanBuffer* srcBuffer,
    VulkanImage* dstImage, VkImageLayout dstImageLayout,
    rad::Span<VkBufferImageCopy> regions)
{
    m_device->GetFunctionTable()->
        vkCmdCopyBufferToImage(m_handle, srcBuffer->GetHandle(), dstImage->GetHandle(), dstImageLayout,
            static_cast<uint32_t>(regions.size()), regions.data());
}

void VulkanCommandBuffer::CopyImageToBuffer(
    VulkanImage* srcImage, VkImageLayout srcImageLayout,
    VulkanBuffer* dstBuffer,
    rad::Span<VkBufferImageCopy> regions)
{
    m_device->GetFunctionTable()->
        vkCmdCopyImageToBuffer(m_handle, srcImage->GetHandle(), srcImageLayout, dstBuffer->GetHandle(),
            static_cast<uint32_t>(regions.size()), regions.data());
}

void VulkanCommandBuffer::BlitImage(
    VulkanImage* srcImage, VkImageLayout srcImageLayout,
    VulkanImage* dstImage, VkImageLayout dstImageLayout,
    rad::Span<VkImageBlit> regions, VkFilter filter)
{
    m_device->GetFunctionTable()->
        vkCmdBlitImage(m_handle, srcImage->GetHandle(), srcImageLayout, dstImage->GetHandle(), dstImageLayout,
            static_cast<uint32_t>(regions.size()), regions.data(), filter);
}

void VulkanCommandBuffer::ResolveImage(
    VulkanImage* srcImage, VkImageLayout srcImageLayout,
    VulkanImage* dstImage, VkImageLayout dstImageLayout,
    rad::Span<VkImageResolve> regions)
{
    m_device->GetFunctionTable()->
        vkCmdResolveImage(m_handle, srcImage->GetHandle(), srcImageLayout, dstImage->GetHandle(), dstImageLayout,
            static_cast<uint32_t>(regions.size()), regions.data());
}

void VulkanCommandBuffer::SetPushConstants(
    VulkanPipelineLayout* layout, VkShaderStageFlags stageFlags,
    uint32_t offset, uint32_t size, const void* pValues)
{
    m_device->GetFunctionTable()->
        vkCmdPushConstants(m_handle, layout->GetHandle(), stageFlags,
            offset, size, pValues);
}

void VulkanCommandBuffer::SetPipelineBarrier(
    VkPipelineStageFlags                srcStageMask,
    VkPipelineStageFlags                dstStageMask,
    VkDependencyFlags                   dependencyFlags,
    rad::Span<VkMemoryBarrier>          memoryBarriers,
    rad::Span<VkBufferMemoryBarrier>    bufferMemoryBarriers,
    rad::Span<VkImageMemoryBarrier>     imageMemoryBarriers)
{
    m_device->GetFunctionTable()->vkCmdPipelineBarrier(m_handle, srcStageMask, dstStageMask, dependencyFlags,
        static_cast<uint32_t>(memoryBarriers.size()), memoryBarriers.data(),
        static_cast<uint32_t>(bufferMemoryBarriers.size()), bufferMemoryBarriers.data(),
        static_cast<uint32_t>(imageMemoryBarriers.size()), imageMemoryBarriers.data()
    );
}

void VulkanCommandBuffer::SetMemoryBarrier(
    VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask,
    VkAccessFlags srcAccessMask, VkAccessFlags dstAccessMask)
{
    VkMemoryBarrier memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    memoryBarrier.pNext = nullptr;
    memoryBarrier.srcAccessMask = srcAccessMask;
    memoryBarrier.dstAccessMask = dstAccessMask;
    this->SetPipelineBarrier(
        srcStageMask,
        dstStageMask,
        0, // DependencyFlags: None
        std::array{ memoryBarrier },
        {},
        {});
}

void VulkanCommandBuffer::SetMemoryBarrier_ComputeToComputeReadAfterWrite()
{
    if (m_bSupportSync2)
    {
        VkMemoryBarrier2KHR memoryBarrier2 = {};
        memoryBarrier2.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR;
        memoryBarrier2.pNext = nullptr;
        memoryBarrier2.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR;
        memoryBarrier2.srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT_KHR;
        memoryBarrier2.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR;
        memoryBarrier2.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT_KHR;
        VkDependencyInfoKHR dependencyInfo = {};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
        dependencyInfo.pNext = nullptr;
        dependencyInfo.dependencyFlags = 0;
        dependencyInfo.memoryBarrierCount = 1;
        dependencyInfo.pMemoryBarriers = &memoryBarrier2;
        m_device->GetFunctionTable()->vkCmdPipelineBarrier2KHR(m_handle, &dependencyInfo);
    }
    else
    {
        VkMemoryBarrier memoryBarrier = {};
        memoryBarrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        memoryBarrier.pNext = nullptr;
        memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        this->SetPipelineBarrier(
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            0, // DependencyFlags: None
            { &memoryBarrier, 1 },
            {},
            {});
    }
}

void VulkanCommandBuffer::SetMemoryBarrier_ComputeToComputeExecutionBarrier()
{
    if (m_bSupportSync2)
    {
        VkMemoryBarrier2KHR memoryBarrier2 = {};
        memoryBarrier2.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR;
        memoryBarrier2.pNext = nullptr;
        memoryBarrier2.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR;
        memoryBarrier2.srcAccessMask = 0;
        memoryBarrier2.dstStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR;
        memoryBarrier2.dstAccessMask = 0;
        VkDependencyInfoKHR dependencyInfo = {};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
        dependencyInfo.pNext = nullptr;
        dependencyInfo.dependencyFlags = 0;
        dependencyInfo.memoryBarrierCount = 1;
        dependencyInfo.pMemoryBarriers = &memoryBarrier2;
        m_device->GetFunctionTable()->vkCmdPipelineBarrier2KHR(m_handle, &dependencyInfo);
    }
    else
    {
        this->SetPipelineBarrier(
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            0, // DependencyFlags: None
            {},
            {},
            {});
    }
}

void VulkanCommandBuffer::SetMemoryBarrier_ComputeToIndexInput()
{
    if (m_bSupportSync2)
    {
        VkMemoryBarrier2KHR memoryBarrier2 = {};
        memoryBarrier2.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER_2_KHR;
        memoryBarrier2.pNext = nullptr;
        memoryBarrier2.srcStageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT_KHR;
        memoryBarrier2.srcAccessMask = VK_ACCESS_2_SHADER_WRITE_BIT_KHR;
        memoryBarrier2.dstStageMask = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT_KHR;
        memoryBarrier2.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT_KHR;
        VkDependencyInfoKHR dependencyInfo = {};
        dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO_KHR;
        dependencyInfo.pNext = nullptr;
        dependencyInfo.dependencyFlags = 0;
        dependencyInfo.memoryBarrierCount = 1;
        dependencyInfo.pMemoryBarriers = &memoryBarrier2;
        m_device->GetFunctionTable()->vkCmdPipelineBarrier2KHR(m_handle, &dependencyInfo);
    }
    else
    {
        SetMemoryBarrier(
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
            VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
            VK_ACCESS_SHADER_WRITE_BIT,
            VK_ACCESS_INDEX_READ_BIT
        );
    }
}

void VulkanCommandBuffer::SetMemoryBarrier_ComputeToDrawIndirect()
{
    SetMemoryBarrier(
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
        VK_ACCESS_SHADER_WRITE_BIT,
        VK_ACCESS_INDIRECT_COMMAND_READ_BIT
    );
}

void VulkanCommandBuffer::SetStorageImageBarrier_ComputeToFragmentShaderSample(VulkanImage* image)
{
    VkImageMemoryBarrier memoryBarrier = {};
    memoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memoryBarrier.pNext = nullptr;
    memoryBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    memoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    memoryBarrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    memoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    memoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memoryBarrier.image = image->GetHandle();
    memoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    memoryBarrier.subresourceRange.baseMipLevel = 0;
    memoryBarrier.subresourceRange.levelCount = image->GetMipLevels();
    memoryBarrier.subresourceRange.baseArrayLayer = 0;
    memoryBarrier.subresourceRange.layerCount = image->GetArrayLayers();
    this->SetPipelineBarrier(
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0, // DependencyFlags: None
        {},
        {},
        { &memoryBarrier, 1 });
}

void VulkanCommandBuffer::SetFullPipelineBarrier()
{
    VkMemoryBarrier memoryBarrier = {};
    memoryBarrier.srcAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
        VK_ACCESS_INDEX_READ_BIT |
        VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
        VK_ACCESS_UNIFORM_READ_BIT |
        VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
        VK_ACCESS_SHADER_READ_BIT |
        VK_ACCESS_SHADER_WRITE_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_TRANSFER_READ_BIT |
        VK_ACCESS_TRANSFER_WRITE_BIT |
        VK_ACCESS_HOST_READ_BIT |
        VK_ACCESS_HOST_WRITE_BIT,
        memoryBarrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
        VK_ACCESS_INDEX_READ_BIT |
        VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
        VK_ACCESS_UNIFORM_READ_BIT |
        VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
        VK_ACCESS_SHADER_READ_BIT |
        VK_ACCESS_SHADER_WRITE_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
        VK_ACCESS_TRANSFER_READ_BIT |
        VK_ACCESS_TRANSFER_WRITE_BIT |
        VK_ACCESS_HOST_READ_BIT |
        VK_ACCESS_HOST_WRITE_BIT;
    this->SetPipelineBarrier(
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, // srcStageMask
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, // dstStageMask
        0, // dependencyFlags
        { &memoryBarrier, 1 },  // pMemoryBarriers
        {},
        {}
    );
}

void VulkanCommandBuffer::TransitLayout(VulkanImage* image,
    VkPipelineStageFlags srcStageMask,
    VkPipelineStageFlags dstStageMask,
    VkAccessFlags srcAccessMask,
    VkAccessFlags dstAccessMask,
    VkImageLayout oldLayout,
    VkImageLayout newLayout,
    const VkImageSubresourceRange* subresourceRange)
{
    VkImageMemoryBarrier imageBarrier = {};
    imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageBarrier.pNext = nullptr;
    imageBarrier.srcAccessMask = srcAccessMask;
    imageBarrier.dstAccessMask = dstAccessMask;
    imageBarrier.oldLayout = oldLayout;
    imageBarrier.newLayout = newLayout;
    imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageBarrier.image = image->GetHandle();
    if (subresourceRange)
    {
        imageBarrier.subresourceRange = *subresourceRange;
    }
    else
    {
        imageBarrier.subresourceRange.aspectMask = VulkanFormat(image->GetFormat()).GetAspectFlags();
        imageBarrier.subresourceRange.baseMipLevel = 0;
        imageBarrier.subresourceRange.levelCount = image->GetMipLevels();
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.subresourceRange.layerCount = image->GetArrayLayers();
    }
    this->SetPipelineBarrier(
        srcStageMask,    // srcStageMask
        dstStageMask,    // dstStageMask
        0,
        {},
        {},
        { &imageBarrier, 1 }
    );
}

void VulkanCommandBuffer::TransitLayoutFromCurrent(
    VulkanImage* image,
    VkPipelineStageFlags    dstStageMask,
    VkAccessFlags           dstAccessMask,
    VkImageLayout           newLayout,
    const VkImageSubresourceRange* subresourceRange)
{
    TransitLayout(image,
        image->GetCurrentPipelineStage(), dstStageMask,
        image->GetCurrentAccessMask(), dstAccessMask,
        image->GetCurrentLayout(), newLayout,
        subresourceRange);
}
