cmake_minimum_required(VERSION 3.10)

project(radcpp
    VERSION 0.0.0.0
    DESCRIPTION "Great C++ collections."
    HOMEPAGE_URL "https://github.com/JerryMa-SDE/radcpp"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(RADCPP_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/imported/
)

add_subdirectory(imported/stb)
set_target_properties(stb PROPERTIES FOLDER "imported")
add_subdirectory(imported/sqlite)
set_target_properties(sqlite PROPERTIES FOLDER "imported")
add_subdirectory(imported/imgui)
set_target_properties(imgui PROPERTIES FOLDER "imported")

add_subdirectory(rad)
add_subdirectory(sp1)

add_subdirectory(libs/DirectMedia)
set_target_properties(DirectMedia PROPERTIES FOLDER "libs")
add_subdirectory(libs/VulkanEngine)
set_target_properties(VulkanEngine PROPERTIES FOLDER "libs")

add_subdirectory(samples/Painter)
