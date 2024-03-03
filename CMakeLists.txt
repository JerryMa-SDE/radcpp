cmake_minimum_required(VERSION 3.10)
project(radcpp)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/imported
)

add_subdirectory(imported/stb)
set_target_properties(stb PROPERTIES FOLDER "imported")

add_subdirectory(rad)
add_subdirectory(tests/HelloWorld)
