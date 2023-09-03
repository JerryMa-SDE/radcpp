set(VULKAN_CORE_SOURCE_FILES
    helpers/vk_enum_string_helper.h
    helpers/vk_format_utils.h
    helpers/vk_format_utils.cpp
    VulkanBuffer.cpp
    VulkanBuffer.h
    VulkanCommandBuffer.cpp
    VulkanCommandBuffer.h
    VulkanCommandPool.cpp
    VulkanCommandPool.h
    VulkanCommon.cpp
    VulkanCommon.h
    VulkanDescriptorPool.cpp
    VulkanDescriptorPool.h
    VulkanDescriptorSet.cpp
    VulkanDescriptorSet.h
    VulkanDevice.cpp
    VulkanDevice.h
    VulkanEvent.cpp
    VulkanEvent.h
    VulkanFence.cpp
    VulkanFence.h
    VulkanFormat.cpp
    VulkanFormat.h
    VulkanFramebuffer.cpp
    VulkanFramebuffer.h
    VulkanImage.cpp
    VulkanImage.h
    VulkanInstance.cpp
    VulkanInstance.h
    VulkanObject.cpp
    VulkanObject.h
    VulkanPhysicalDevice.cpp
    VulkanPhysicalDevice.h
    VulkanPipeline.cpp
    VulkanPipeline.h
    VulkanQueue.cpp
    VulkanQueue.h
    VulkanRenderPass.cpp
    VulkanRenderPass.h
    VulkanSampler.cpp
    VulkanSampler.h
    VulkanSemaphore.cpp
    VulkanSemaphore.h
    VulkanShader.cpp
    VulkanShader.h
    VulkanSwapchain.cpp
    VulkanSwapchain.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VULKAN_CORE_SOURCE_FILES})

add_library(VulkanCore
    ${VULKAN_CORE_SOURCE_FILES}
)

target_include_directories(VulkanCore
    PUBLIC ${RADCPP_SOURCE_DIR}/3rdparty/Vulkan-Headers/include/
    PUBLIC ${RADCPP_SOURCE_DIR}/3rdparty/volk/
    PUBLIC ${RADCPP_SOURCE_DIR}/3rdparty/VulkanMemoryAllocator/include/
    PUBLIC ${RADCPP_SOURCE_DIR}/3rdparty/spvgen/include/
)

target_link_libraries(VulkanCore
    PUBLIC rad
)
