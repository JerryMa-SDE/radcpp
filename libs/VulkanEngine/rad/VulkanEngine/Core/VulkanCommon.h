#ifndef VULKAN_COMMON_H
#define VULKAN_COMMON_H

#include "rad/Core/Integer.h"
#include "rad/Core/Flags.h"
#include "rad/Core/RefCounted.h"
#include "rad/Core/Span.h"
#include "rad/IO/Logging.h"

#define VK_NO_PROTOTYPES
#include "volk.h"

// Fetch pointers to Vulkan functions dynamically using vkGetInstanceProcAddr, vkGetDeviceProcAddr.
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include "vk_mem_alloc.h"

#include "Internal/vk_enum_string_helper.h"
#include <exception>

extern rad::LogCategory g_logVulkan;
#define LogVulkan(Level, Format, ...) g_logVulkan.Print(rad::LogLevel::Level, Format, ##__VA_ARGS__)

class VulkanError : public std::exception
{
public:
    VulkanError(VkResult result) : m_result(result) {}
    const char* what() const noexcept override { return string_VkResult(m_result); }
private:
    VkResult m_result;
}; // class VulkanError

// Check Vulkan return code and throw VulkanError if result < 0
void vkCheckResult(VkResult result, const char* function, const char* file, uint32_t line);

#define VK_CHECK(VulkanCall) \
do { const VkResult r = VulkanCall; vkCheckResult(r, #VulkanCall, __FILE__, __LINE__); } while(0)

enum VulkanQueueFamily : uint32_t
{
    VulkanQueueFamilyUniversal = 0, // Default Family that supports all features
    VulkanQueueFamilyCompute,       // Async Compute Engine (ACE)
    VulkanQueueFamilyTransfer,      // DMA
    VulkanQueueFamilyCount
};

class VulkanVersion
{
public:
    VulkanVersion(uint32_t version);
    VulkanVersion(uint32_t major, uint32_t minor, uint32_t patch);
    ~VulkanVersion();

    uint32_t GetMajor() const { return VK_VERSION_MAJOR(m_version); }
    uint32_t GetMinor() const { return VK_VERSION_MINOR(m_version); }
    uint32_t GetPatch() const { return VK_VERSION_PATCH(m_version); }
    std::string GetString() const;

    operator uint32_t() const { return m_version; }

private:
    uint32_t m_version = 0;
}; // class VulkanVersion

// Construct structure chain (preserve order): Head->S1->S2->S3
// VK_STRUCTURE_CHAIN_BEGIN(Head); // will create a temporary var Head##ppNext
// Append structures dynamically:
// VK_STRUCTURE_CHAIN_ADD(Head, S1); // push back
// VK_STRUCTURE_CHAIN_ADD(Head, S2);
// VK_STRUCTURE_CHAIN_ADD(Head, S3);
// Set the last pNext to nullptr, terminate the chain:
// VK_STRUCTURE_CHAIN_END(Head);
#define VK_STRUCTURE_CHAIN_BEGIN(Head) \
void** Head##Chain##_ppNext = (void**)(&Head.pNext);
#define VK_STRUCTURE_CHAIN_ADD(Head, Next) \
do { *Head##Chain##_ppNext = (void*)(&Next); Head##Chain##_ppNext = (void**)(&Next.pNext); } while (0)
#define VK_STRUCTURE_CHAIN_END(Head) \
*Head##Chain##_ppNext = nullptr;

// Search from head, find the tail, append the new structure dynamically.
template<typename Head, typename Last>
void vkStructureChainAppend(Head& head, Last& last)
{
    VkBaseOutStructure* iter = reinterpret_cast<VkBaseOutStructure*>(&head);
    // Search the end:
    while (iter->pNext != nullptr)
    {
        iter = reinterpret_cast<VkBaseOutStructure*>(iter->pNext);
    }
    // Append the new structure:
    iter->pNext = reinterpret_cast<VkBaseOutStructure*>(&last);
    last.pNext = nullptr; // terminate the chain.
}

template<typename Head, typename Last>
void vkStructureChainInsert(Head& head, Last& next)
{
    VkBaseOutStructure* pHeadBase = reinterpret_cast<VkBaseOutStructure*>(&head);
    VkBaseOutStructure* pNextBase = reinterpret_cast<VkBaseOutStructure*>(&next);
    pNextBase->pNext = pHeadBase->pNext;
    pHeadBase->pNext = (void*)(&next);
}

class VulkanObject;
class VulkanInstance;
class VulkanPhysicalDevice;
class VulkanDevice;
class VulkanQueue;
class VulkanCommandBuffer;
class VulkanCommandPool;
class VulkanFence;
class VulkanSemaphore;
class VulkanEvent;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanShader;
class VulkanShaderModule;
class VulkanPipeline;
class VulkanComputePipeline;
class VulkanGraphicsPipeline;
class VulkanBuffer;
class VulkanBufferView;
class VulkanImage;
class VulkanImageView;
class VulkanSampler;
class VulkanDescriptorPool;
class VulkanDescriptorSet;
class VulkanDescriptorSetLayout;
class VulkanPipelineLayout;
class VulkanSwapchain;

struct VulkanGraphicsPipelineCreateInfo : public rad::RefCounted<VulkanGraphicsPipelineCreateInfo>
{
    VulkanGraphicsPipelineCreateInfo();
    ~VulkanGraphicsPipelineCreateInfo();

    void AddVertexBinding(uint32_t binding, uint32_t stride,
        VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);
    void AddVertexAttribute(uint32_t location, uint32_t binding, VkFormat format, uint32_t offset);
    // assume the vertex attributes are compacted in order.
    void AddVertexBindingWithFormats(uint32_t binding, rad::Span<VkFormat> formats,
        VkVertexInputRate inputRate = VK_VERTEX_INPUT_RATE_VERTEX);

    std::vector<rad::Ref<VulkanShader>> m_shaders;

    struct VertexInputInfo
    {
        std::vector<VkVertexInputBindingDescription> bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;
    } m_vertexInput;

    struct InputAssemblyInfo
    {
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkBool32 primitiveRestartEnable = VK_FALSE;
    } m_inputAssembly;

    struct TessellationInfo
    {
        uint32_t patchControlPoints = 0;
    } m_tessellation;

    // viewports and scissors should be set dynamically.
    uint32_t m_viewportCount = 1;
    uint32_t m_scissorCount = 1;

    struct RasterizationInfo
    {
        VkBool32                depthClampEnable = VK_FALSE;
        VkBool32                rasterizerDiscardEnable = VK_FALSE;
        VkPolygonMode           polygonMode = VK_POLYGON_MODE_FILL;
        VkCullModeFlags         cullMode = VK_CULL_MODE_NONE;
        VkFrontFace             frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        VkBool32                depthBiasEnable = VK_FALSE;
        float                   depthBiasConstantFactor = 0;
        float                   depthBiasClamp = 0;
        float                   depthBiasSlopeFactor = 0;
        float                   lineWidth = 1.0f;
    } m_rasterization;

    struct MultisampleInfo
    {
        VkSampleCountFlagBits   rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        VkBool32                sampleShadingEnable = VK_FALSE;
        float                   minSampleShading = 0.0f;    // must be in the range [0,1]
        VkSampleMask            sampleMask = 0xFFFFFFFF;
        VkBool32                alphaToCoverageEnable = VK_FALSE;
        VkBool32                alphaToOneEnable = VK_FALSE;
    } m_multisample;

    struct DepthStencilInfo
    {
        VkBool32                depthTestEnable = VK_FALSE;
        VkBool32                depthWriteEnable = VK_FALSE;
        VkCompareOp             depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        VkBool32                depthBoundsTestEnable = VK_FALSE;
        VkBool32                stencilTestEnable = VK_FALSE;
        VkStencilOpState        front;
        VkStencilOpState        back;
        float                   minDepthBounds = 0.0f;
        float                   maxDepthBounds = 1.0f;
    } m_depthStencil;

    struct ColorBlendInfo
    {
        VkBool32                logicOpEnable = VK_FALSE;
        VkLogicOp               logicOp;
        std::vector<VkPipelineColorBlendAttachmentState> attachments;
        float                   blendConstants[4];
    } m_colorBlend;

    void SetColorBlendDisabled(uint32_t attachCount);

    rad::Ref<VulkanPipelineLayout>  m_layout;
    rad::Ref<VulkanRenderPass>      m_renderPass;
    uint32_t                        m_subpass = 0;
    rad::Ref<VulkanPipeline>        m_basePipeline;
    int32_t                         m_basePipelineIndex = 0;

    VkPipelineRenderingCreateInfoKHR m_renderingInfo = {};
    void SetRenderingInfo(rad::Span<VkFormat> colorFormats, VkFormat depthStencilFormat = VK_FORMAT_UNDEFINED);

}; // struct VulkanGraphicsPipelineCreateInfo

struct VulkanComputePipelineCreateInfo : public rad::RefCounted<VulkanComputePipelineCreateInfo>
{
    rad::Ref<VulkanShader>          shader;
    rad::Ref<VulkanPipelineLayout>  layout;
    rad::Ref<VulkanPipeline>        basePipeline;
    int32_t                         basePipelineIndex = 0;

}; // struct VulkanComputePipelineCreateInfo

#endif // VULKAN_COMMON_H
