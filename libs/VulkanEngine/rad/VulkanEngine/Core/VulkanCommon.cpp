#define VOLK_IMPLEMENTATION
#define VMA_IMPLEMENTATION

#include "VulkanCommon.h"
#include "VulkanFormat.h"
#include "VulkanShader.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"

rad::LogCategory g_logVulkan("Vulkan", rad::LogLevel::Info);

void VulkanErrorHandler(VkResult result, const char* function, const char* file, uint32_t line)
{
    LogVulkan(Error, "%s failed with VkResult=%s(%d).",
        function, string_VkResult(result), result, file, line);
    throw VulkanError(result);
}

VulkanVersion::VulkanVersion(uint32_t version) :
    m_version(version)
{
}

VulkanVersion::VulkanVersion(uint32_t major, uint32_t minor, uint32_t patch)
{
    m_version = VK_MAKE_VERSION(major, minor, patch);
}

VulkanVersion::~VulkanVersion()
{
}

std::string VulkanVersion::GetString() const
{
    return rad::StrPrint("%u.%u.%u",
        VK_VERSION_MAJOR(m_version),
        VK_VERSION_MINOR(m_version),
        VK_VERSION_PATCH(m_version)
    );
}

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

void VulkanGraphicsPipelineCreateInfo::AddVertexBinding(
    uint32_t binding, uint32_t stride, VkVertexInputRate inputRate)
{
    VkVertexInputBindingDescription vertexBindingDesc = {};
    vertexBindingDesc.binding = binding;
    vertexBindingDesc.stride = stride;
    vertexBindingDesc.inputRate = inputRate;
    m_vertexInput.bindings.push_back(vertexBindingDesc);
}

void VulkanGraphicsPipelineCreateInfo::AddVertexAttribute(
    uint32_t location, uint32_t binding, VkFormat format, uint32_t offset)
{
    VkVertexInputAttributeDescription vertexAttribDesc = {};
    vertexAttribDesc.location = location;
    vertexAttribDesc.binding = binding;
    vertexAttribDesc.format = format;
    vertexAttribDesc.offset = offset;
    m_vertexInput.attributes.push_back(vertexAttribDesc);
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
        vertexBindingDesc.stride += VulkanFormat(format).GetElementSize();
    }

    m_vertexInput.bindings.push_back(vertexBindingDesc);
}

void VulkanGraphicsPipelineCreateInfo::SetColorBlendDisabled(uint32_t attachCount)
{
    m_colorBlend.attachments.resize(attachCount);
    for (VkPipelineColorBlendAttachmentState& state : m_colorBlend.attachments)
    {
        state.blendEnable = VK_FALSE;
        state.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;
    }
}
