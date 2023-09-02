set(VULKAN_CORE_SOURCE_FILES
    helpers/vk_enum_string_helper.h
    helpers/vk_format_utils.h
    helpers/vk_format_utils.cpp
    VulkanCommon.h
    VulkanCommon.cpp
    VulkanFormat.h
    VulkanFormat.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${VULKAN_CORE_SOURCE_FILES})

add_library(VulkanCore
    ${VULKAN_CORE_SOURCE_FILES}
)

target_include_directories(VulkanCore
    PUBLIC ${RADCPP_SOURCE_DIR}/3rdparty/Vulkan-Headers/include/
    PUBLIC ${RADCPP_SOURCE_DIR}/3rdparty/volk/
    PUBLIC ${RADCPP_SOURCE_DIR}/3rdparty/VulkanMemoryAllocator/include/
)

target_link_libraries(VulkanCore
    PUBLIC rad
)
