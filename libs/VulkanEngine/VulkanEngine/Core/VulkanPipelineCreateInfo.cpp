#include "VulkanPipelineCreateInfo.h"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"

VulkanGraphicsPipelineCreateInfo::VulkanGraphicsPipelineCreateInfo()
{
    VkStencilOpState stencilOpState = {};
    stencilOpState.failOp = VK_STENCIL_OP_KEEP;
    stencilOpState.passOp = VK_STENCIL_OP_KEEP;
    stencilOpState.depthFailOp;
    stencilOpState.compareOp = VK_COMPARE_OP_ALWAYS;
    stencilOpState.compareMask;
    stencilOpState.writeMask;
    stencilOpState.reference;

    m_depthStencil.front = stencilOpState;
    m_depthStencil.back = stencilOpState;
}

VulkanGraphicsPipelineCreateInfo::~VulkanGraphicsPipelineCreateInfo()
{
}

void VulkanGraphicsPipelineCreateInfo::AddVertexBindingWithFormats(
    uint32_t binding, rad::Span<VkFormat> formats, VkVertexInputRate inputRate)
{
    VkVertexInputBindingDescription vertexBindingDesc = {};
    vertexBindingDesc.binding = binding;
    vertexBindingDesc.stride = 0;
    vertexBindingDesc.inputRate = inputRate;
    uint32_t location = 0;
    for (VkFormat format : formats)
    {
        VkVertexInputAttributeDescription vertexAttribDesc = {};
        vertexAttribDesc.location = location;
        vertexAttribDesc.binding = binding;
        vertexAttribDesc.format = format;
        vertexAttribDesc.offset = vertexBindingDesc.stride;
        m_vertexInput.attributes.push_back(vertexAttribDesc);

        ++location;
        vertexBindingDesc.stride += vkuFormatElementSize(format);
    }
    m_vertexInput.bindings.push_back(vertexBindingDesc);
}

void VulkanGraphicsPipelineCreateInfo::SetColorBlendDisabled(uint32_t attachCount)
{
    m_colorBlend.attachments.resize(attachCount);
    for (auto& attachment : m_colorBlend.attachments)
    {
        attachment = {};
        attachment.blendEnable = VK_FALSE;
        attachment.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
    }
}

void VulkanGraphicsPipelineCreateInfo::SetRenderingInfo(
    rad::Span<VkFormat> colorFormats, VkFormat depthStencilFormat)
{
    m_renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
    m_renderingInfo.colorAttachmentCount = uint32_t(colorFormats.size());
    m_renderingInfo.pColorAttachmentFormats = colorFormats.data();
    m_renderingInfo.depthAttachmentFormat = depthStencilFormat;
    m_renderingInfo.stencilAttachmentFormat = depthStencilFormat;
}
