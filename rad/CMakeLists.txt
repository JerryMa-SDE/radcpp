set(rad_HEADERS
    Core/Global.h
    Core/Integer.h
    Core/Float.h
    Core/Float16Compressor.h
    Core/String.h
    Core/Flags.h
    Core/RefCounted.h
    Core/Memory.h
    Core/Time.h
    Core/TypeTraits.h
    Container/Span.h
    IO/File.h
    IO/Logging.h
    IO/Json.h
    System/FileSystem.h
    System/OS.h
    Math/Math.h
    Math/3DLinearAlgebra.h
)

set(rad_SOURCES ${rad_HEADERS}
    Core/Global.cpp
    Core/Integer.cpp
    Core/Float.cpp
    Core/String.cpp
    Core/Memory.cpp
    Core/Time.cpp
    IO/File.cpp
    IO/Logging.cpp
    IO/Json.cpp
    System/FileSystem.cpp
    System/OS.cpp
    Math/Math.cpp
    Math/3DLinearAlgebra.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${rad_SOURCES})

add_library(rad STATIC ${rad_SOURCES})

find_package(Boost REQUIRED json)
find_package(spdlog CONFIG REQUIRED)
find_package(CpuFeatures CONFIG REQUIRED)

target_include_directories(rad
    PUBLIC ${Boost_INCLUDE_DIRS}
    PUBLIC ${RADCPP_SOURCE_DIR}/imported/ms-gsl/include
)

target_link_libraries(rad
    PUBLIC ${Boost_LIBRARIES}
    PUBLIC stb
    PUBLIC spdlog::spdlog
    PUBLIC CpuFeatures::cpu_features
)

if (WIN32)
target_compile_definitions(rad
    PRIVATE WIN32_LEAN_AND_MEAN=1
    PRIVATE NOMINMAX=1
)
target_compile_options(rad
    PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/utf-8>"
)
endif()
