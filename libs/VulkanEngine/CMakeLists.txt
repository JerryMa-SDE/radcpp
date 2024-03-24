set(VulkanEngine_HEADERS
    VulkanEngine/Core/VulkanCommon.h
    VulkanEngine/Core/VulkanObject.h
    VulkanEngine/Core/VulkanInstance.h
    VulkanEngine/Core/VulkanPhysicalDevice.h
    VulkanEngine/Core/VulkanDevice.h
    VulkanEngine/Core/VulkanQueue.h
    VulkanEngine/Core/VulkanCommandPool.h
    VulkanEngine/Core/VulkanCommandBuffer.h
    VulkanEngine/Core/VulkanFence.h
    VulkanEngine/Core/VulkanSemaphore.h
    VulkanEngine/Core/VulkanEvent.h
    VulkanEngine/Core/VulkanRenderPass.h
    VulkanEngine/Core/VulkanFramebuffer.h
    VulkanEngine/Core/VulkanShader.h
    VulkanEngine/Core/VulkanPipeline.h
    VulkanEngine/Core/VulkanPipelineCreateInfo.h
    VulkanEngine/Core/VulkanBuffer.h
    VulkanEngine/Core/VulkanImage.h
    VulkanEngine/Core/VulkanSampler.h
    VulkanEngine/Core/VulkanDescriptor.h
    VulkanEngine/Core/VulkanSwapchain.h
)

set(VulkanEngine_SOURCES ${VulkanEngine_HEADERS}
    VulkanEngine/Core/VulkanCommon.cpp
    VulkanEngine/Core/VulkanObject.cpp
    VulkanEngine/Core/VulkanInstance.cpp
    VulkanEngine/Core/VulkanPhysicalDevice.cpp
    VulkanEngine/Core/VulkanDevice.cpp
    VulkanEngine/Core/VulkanQueue.cpp
    VulkanEngine/Core/VulkanCommandPool.cpp
    VulkanEngine/Core/VulkanCommandBuffer.cpp
    VulkanEngine/Core/VulkanFence.cpp
    VulkanEngine/Core/VulkanSemaphore.cpp
    VulkanEngine/Core/VulkanEvent.cpp
    VulkanEngine/Core/VulkanRenderPass.cpp
    VulkanEngine/Core/VulkanFramebuffer.cpp
    VulkanEngine/Core/VulkanShader.cpp
    VulkanEngine/Core/VulkanPipeline.cpp
    VulkanEngine/Core/VulkanPipelineCreateInfo.cpp
    VulkanEngine/Core/VulkanBuffer.cpp
    VulkanEngine/Core/VulkanImage.cpp
    VulkanEngine/Core/VulkanSampler.cpp
    VulkanEngine/Core/VulkanDescriptor.cpp
    VulkanEngine/Core/VulkanSwapchain.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR}/VulkanEngine/ FILES ${VulkanEngine_SOURCES})

add_library(VulkanEngine
    ${VulkanEngine_SOURCES}
)

target_include_directories(VulkanEngine
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
    PUBLIC ${RADCPP_SOURCE_DIR}/imported/Vulkan-Headers/include/
    PUBLIC ${RADCPP_SOURCE_DIR}/imported/volk/
    PUBLIC ${RADCPP_SOURCE_DIR}/imported/VulkanMemoryAllocator/include/
    PUBLIC ${RADCPP_SOURCE_DIR}/imported/spvgen/include/
)

target_link_libraries(VulkanEngine
    PUBLIC rad
    PUBLIC volk
)

if(WIN32)
target_compile_definitions(VulkanEngine
    PRIVATE WIN32_LEAN_AND_MEAN=1
    PRIVATE NOMINMAX=1
)
target_compile_options(VulkanEngine
    PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
)
add_custom_command(TARGET VulkanEngine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${RADCPP_SOURCE_DIR}/imported/spvgen/build/Release/spvgen.dll
        ${CMAKE_CURRENT_BINARY_DIR}
)
endif()
