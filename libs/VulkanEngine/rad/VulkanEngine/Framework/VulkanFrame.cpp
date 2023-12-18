#include "VulkanFrame.h"
#include "VulkanWindow.h"

VulkanFrame::VulkanFrame(VulkanWindow* window) :
    m_window(window)
{
}

VulkanFrame::~VulkanFrame()
{
}

bool VulkanFrame::Init(VulkanDevice* device)
{
    m_device = device;
    m_backBufferCount = 3; // desires to acquire 3 images for tripple buffering
    m_commandPool = m_device->CreateCommandPool(VulkanQueueFamilyUniversal,
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    CreateCommandBuffers();

    for (uint32_t i = 0; i < FrameLag; i++)
    {
        m_swapchainImageAcquired[i] = m_device->CreateSemaphore();
        m_drawComplete[i] = m_device->CreateSemaphore();
        m_frameThrottles[i] = m_device->CreateFence(VK_FENCE_CREATE_SIGNALED_BIT);
    }

    int width = 0;
    int height = 0;
    m_window->GetDrawableSize(&width, &height);
    Resize(width, height);

    CreateSampler();
    CreatePipeline();

    return true;
}

void VulkanFrame::Resize(uint32_t width, uint32_t height)
{
    m_device->WaitIdle();
    CreateSwapchain(width, height);
    if (m_commandBuffers.size() < m_swapchain->GetImageCount())
    {
        CreateCommandBuffers();
    }
    CreateRenderPass();
    CreateFramebuffers(width, height);
}

void VulkanFrame::SetColorBuffer(rad::Ref<VulkanImage> colorBuffer)
{
    if (m_colorBuffer != colorBuffer)
    {
        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.pNext = nullptr;
        write.dstSet = m_descriptorSet->GetHandle();
        write.dstBinding = 0;
        write.dstArrayElement = 0;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        VkDescriptorImageInfo imageInfo = {};
        imageInfo.sampler = m_sampler->GetHandle();
        imageInfo.imageView = colorBuffer->GetDefaultView()->GetHandle();
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        write.pImageInfo = &imageInfo;
        m_descriptorSet->Update(write);
        m_colorBuffer = std::move(colorBuffer);
    }
}

void VulkanFrame::SetOverlay(rad::Ref<VulkanImage> overlay)
{
    if (m_overlay != overlay)
    {
        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.pNext = nullptr;
        write.dstSet = m_descriptorSet->GetHandle();
        write.dstBinding = 1;
        write.dstArrayElement = 0;
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        VkDescriptorImageInfo imageInfo = {};
        imageInfo.sampler = m_sampler->GetHandle();
        imageInfo.imageView = overlay->GetDefaultView()->GetHandle();
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        write.pImageInfo = &imageInfo;
        m_descriptorSet->Update(write);
        m_overlay = std::move(overlay);
    }
}

uint32_t VulkanFrame::Begin()
{
    m_frameThrottles[m_frameIndex]->Wait();
    m_frameThrottles[m_frameIndex]->Reset();
    uint32_t backBufferIndex = m_swapchain->AcquireNextImage(m_swapchainImageAcquired[m_frameIndex].get());
    return backBufferIndex;
}

void VulkanFrame::End()
{
    RenderToSwapchain();
    Present();
}

void VulkanFrame::RenderToSwapchain()
{
    uint32_t backBufferIndex = m_swapchain->GetCurrentImageIndex();
    VulkanCommandBuffer* cmdBuffer = m_commandBuffers[backBufferIndex].get();

    cmdBuffer->Begin();

    if (m_colorBuffer->GetCurrentLayout() != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        cmdBuffer->TransitLayoutFromCurrent(m_colorBuffer.get(),
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    if (m_overlay->GetCurrentLayout() != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        cmdBuffer->TransitLayoutFromCurrent(m_overlay.get(),
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_ACCESS_SHADER_READ_BIT,
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }

    VkClearValue clearValues[1] = {};
    clearValues[0].color = {};
    cmdBuffer->BeginRenderPass(m_renderPass.get(), m_framebuffers[backBufferIndex].get(),
        clearValues);
    cmdBuffer->BindPipeline(m_pipeline.get());
    cmdBuffer->BindDescriptorSets(m_pipeline.get(), m_pipelineLayout.get(), 0, m_descriptorSet.get());

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = float(m_swapchain->GetWidth());
    viewport.height = float(m_swapchain->GetHeight());
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    cmdBuffer->SetViewports(viewport);

    VkRect2D scissor = {};
    scissor.extent.width = m_swapchain->GetWidth();
    scissor.extent.height = m_swapchain->GetHeight();
    cmdBuffer->SetScissors(scissor);

    cmdBuffer->Draw(3, 1, 0, 0);

    cmdBuffer->EndRenderPass();
    cmdBuffer->End();

    m_device->GetQueue()->Submit(
        std::array{
            cmdBuffer
        },
        std::array{ // wait
            VulkanSubmitWait
            {
                m_swapchainImageAcquired[m_frameIndex].get(),
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
            }
        },
        std::array{ // signal
            m_drawComplete[m_frameIndex].get()
        },
        m_frameThrottles[m_frameIndex].get()
    );
}

void VulkanFrame::Present()
{
    m_device->GetQueue()->Present(
        std::array{ // wait
            m_drawComplete[m_frameIndex].get()
        },
        std::array{
            m_swapchain.get()
        },
        std::array{
            m_swapchain->GetCurrentImageIndex()
        }
    );

    m_frameIndex += 1;
    m_frameIndex %= FrameLag;
}

VkSurfaceFormatKHR GetSurfaceFormat(rad::Span<VkSurfaceFormatKHR> candidateFormats)
{
    assert(candidateFormats.size() >= 1);

    VkSurfaceFormatKHR defaultFormat = {};
    defaultFormat.format = VK_FORMAT_R8G8B8A8_UNORM;
    defaultFormat.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    if (candidateFormats.empty())
    {
        return defaultFormat;
    }

    if ((candidateFormats.size() == 1) && (candidateFormats[0].format == VK_FORMAT_UNDEFINED))
    {
        return defaultFormat;
    }

    for (size_t i = 0; i < candidateFormats.size(); i++)
    {
        const VkFormat format = candidateFormats[i].format;

        if (format == VK_FORMAT_R8G8B8A8_UNORM || format == VK_FORMAT_B8G8R8A8_UNORM ||
            format == VK_FORMAT_A2B10G10R10_UNORM_PACK32 || format == VK_FORMAT_A2R10G10B10_UNORM_PACK32 ||
            format == VK_FORMAT_R16G16B16A16_SFLOAT)
        {
            return candidateFormats[i];
        }
    }

    LogVulkan(Warn, "Can't find preferred surface format, falling back to the first exposed format %s (%s)!",
        string_VkFormat(candidateFormats[0].format), string_VkColorSpaceKHR(candidateFormats[0].colorSpace));
    return candidateFormats[0];
}

bool VulkanFrame::CreateCommandBuffers()
{
    m_commandBuffers.resize(m_backBufferCount);
    for (uint32_t i = 0; i < m_backBufferCount; i++)
    {
        m_commandBuffers[i] = m_commandPool->Allocate();
    }
    return true;
}

bool VulkanFrame::CreateSwapchain(uint32_t width, uint32_t height)
{
    if (m_swapchain &&
        (m_swapchain->GetWidth() == width) &&
        (m_swapchain->GetHeight() == height))
    {
        return true;
    }

    VkSurfaceKHR surfaceHandle = m_window->GetSurfaceHandle();
    m_surfaceCapabilities = m_device->GetPhysicalDevice()->GetSurfaceCapabilities(surfaceHandle);
    m_surfaceFormats = m_device->GetPhysicalDevice()->GetSurfaceFormats(surfaceHandle);
    m_presentModes = m_device->GetPhysicalDevice()->GetSurfacePresentModes(surfaceHandle);

    // width and height are either both 0xFFFFFFFF, or both not 0xFFFFFFFF.
    if (m_surfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
    {
        // If the surface size is undefined, the size is set to the size of the images requested,
        // which must fit within the minimum and maximum values.
        if (width < m_surfaceCapabilities.minImageExtent.width)
        {
            width = m_surfaceCapabilities.minImageExtent.width;
        }
        else if (width > m_surfaceCapabilities.maxImageExtent.width)
        {
            width = m_surfaceCapabilities.maxImageExtent.width;
        }

        if (height < m_surfaceCapabilities.minImageExtent.height)
        {
            height = m_surfaceCapabilities.minImageExtent.height;
        }
        else if (height > m_surfaceCapabilities.minImageExtent.height)
        {
            height = m_surfaceCapabilities.minImageExtent.height;
        }
    }
    else
    {
        // If the surface size is defined, the swap chain size must match
        width = m_surfaceCapabilities.minImageExtent.width;
        height = m_surfaceCapabilities.minImageExtent.height;
    }

    if (width == 0 || height == 0)
    {
        LogVulkan(Info, "VulkanFrame::CreateSwapchain(width=%u, height=%u): minimized?",
            width, height);
        return true;
    }

    if (m_backBufferCount < m_surfaceCapabilities.minImageCount)
    {
        m_backBufferCount = m_surfaceCapabilities.minImageCount;
    }
    if ((m_surfaceCapabilities.maxImageCount > 0) &&
        (m_backBufferCount > m_surfaceCapabilities.maxImageCount))
    {
        m_backBufferCount = m_surfaceCapabilities.maxImageCount;
    }

    m_surfaceFormat = GetSurfaceFormat(m_surfaceFormats);
    LogVulkan(Info, "Select SurfaceFormat %s (%s)",
        string_VkFormat(m_surfaceFormat.format), string_VkColorSpaceKHR(m_surfaceFormat.colorSpace));

    VkSurfaceTransformFlagBitsKHR preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    if (m_surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        preTransform = m_surfaceCapabilities.currentTransform;
    }

    rad::Flags32<VkCompositeAlphaFlagBitsKHR> supportedCompositeAlpha(m_surfaceCapabilities.supportedCompositeAlpha);
    VkCompositeAlphaFlagBitsKHR compositeAlpha = supportedCompositeAlpha.HasBits(VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) ?
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR : VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    if (supportedCompositeAlpha.HasBits(VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR))
    {
        compositeAlpha = VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR;
    }
    else if (supportedCompositeAlpha.HasBits(VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR))
    {
        compositeAlpha = VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR;
    }

    bool presentModeSupported = false;
    for (size_t i = 0; i < m_presentModes.size(); ++i)
    {
        if (m_presentMode == m_presentModes[i])
        {
            presentModeSupported = true;
            break;
        }
    }

    if (!presentModeSupported)
    {
        LogVulkan(Warn, "PresentMode %s is not supported, choose %s instead!\n",
            string_VkPresentModeKHR(m_presentMode), string_VkPresentModeKHR(VK_PRESENT_MODE_FIFO_KHR));
        m_presentMode = VK_PRESENT_MODE_FIFO_KHR;
    }

    VkSwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.pNext = nullptr;
    swapchainInfo.flags = 0;
    swapchainInfo.surface = surfaceHandle;
    swapchainInfo.minImageCount = m_backBufferCount;
    swapchainInfo.imageFormat = m_surfaceFormat.format;
    swapchainInfo.imageColorSpace = m_surfaceFormat.colorSpace;
    swapchainInfo.imageExtent.width = width;
    swapchainInfo.imageExtent.height = height;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainInfo.queueFamilyIndexCount;
    swapchainInfo.pQueueFamilyIndices;
    swapchainInfo.preTransform = preTransform;
    swapchainInfo.compositeAlpha = compositeAlpha;
    swapchainInfo.presentMode = m_presentMode;
    swapchainInfo.clipped = VK_TRUE;
    swapchainInfo.oldSwapchain = m_swapchain ? m_swapchain->GetHandle() : VK_NULL_HANDLE;

    m_swapchain = m_device->CreateSwapchain(swapchainInfo);
    LogVulkan(Info, "VulkanSwapchain (re)created (0x%p): extent=%ux%u;",
        m_swapchain->GetHandle(), m_swapchain->GetWidth(), m_swapchain->GetHeight());
    return true;
}

bool VulkanFrame::CreateRenderPass()
{
    std::array<VkAttachmentDescription, 1> attachments = {};
    attachments[0].flags = 0;
    attachments[0].format = m_surfaceFormat.format;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorRef = {};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDesc = {};
    subpassDesc.flags = 0;
    subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDesc.inputAttachmentCount = 0;
    subpassDesc.pInputAttachments = nullptr;
    subpassDesc.colorAttachmentCount = 1;
    subpassDesc.pColorAttachments = &colorRef;
    subpassDesc.pResolveAttachments = nullptr;
    subpassDesc.pDepthStencilAttachment = nullptr;
    subpassDesc.preserveAttachmentCount = 0;
    subpassDesc.pPreserveAttachments = nullptr;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.pNext = nullptr;
    renderPassInfo.flags = 0;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDesc;
    renderPassInfo.dependencyCount = 0;
    renderPassInfo.pDependencies = nullptr;

    m_renderPass = m_device->CreateRenderPass(renderPassInfo);
    return true;
}

bool VulkanFrame::CreateFramebuffers(uint32_t width, uint32_t height)
{
    m_framebuffers.resize(m_backBufferCount);
    for (int i = 0; i < m_framebuffers.size(); i++)
    {
        m_framebuffers[i] = m_device->CreateFramebuffer(m_renderPass.get(),
            m_swapchain->GetDefaultView(i),
            width, height, 1);
    }
    return true;
}

bool VulkanFrame::CreateSampler()
{
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.mipLodBias = 0;
    samplerInfo.minLod = 0;
    samplerInfo.maxLod = 0;
    m_sampler = m_device->CreatSampler(samplerInfo);
    return true;
}

bool VulkanFrame::CreatePipeline()
{
    std::vector<VkDescriptorSetLayoutBinding> descBindings =
    { // binding, type, count, stageFlags, samplers
        { 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
        { 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr },
    };
    m_descriptorSetLayout = m_device->CreateDescriptorSetLayout(descBindings);
    m_pipelineLayout = m_device->CreatePipelineLayout(m_descriptorSetLayout.get());

    rad::Ref<VulkanGraphicsPipelineCreateInfo> pipelineInfo = new VulkanGraphicsPipelineCreateInfo();
    pipelineInfo->m_inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelineInfo->SetColorBlendDisabled(0);
    pipelineInfo->m_layout = m_pipelineLayout;
    pipelineInfo->m_renderPass = m_renderPass;

    auto vert = VulkanShader::CreateFromFile(VK_SHADER_STAGE_VERTEX_BIT, "Shaders/Framework/RenderToSwapchain.vert");
    auto frag = VulkanShader::CreateFromFile(VK_SHADER_STAGE_FRAGMENT_BIT, "Shaders/Framework/RenderToSwapchain.frag");

    pipelineInfo->m_shaders = { vert, frag };
    m_pipeline = m_device->CreateGraphicsPipeline(pipelineInfo);

    m_descriptorPool = m_device->CreateDescriptorPool(1,  // maxSets
        { // type, count
            VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2 },
        }
    );
    m_descriptorSet = m_descriptorPool->Allocate(m_descriptorSetLayout.get());

    return true;
}
