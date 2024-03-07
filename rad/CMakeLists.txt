set(RAD_SOURCES
    Core/Global.h
    Core/Global.cpp
    Core/Integer.h
    Core/Integer.cpp
    Core/Float.h
    Core/Float.cpp
    Core/Float16Compressor.h
    Core/String.h
    Core/String.cpp
    Core/Flags.h
    Core/RefCounted.h
    Core/Memory.h
    Core/Memory.cpp
    Core/TypeTraits.h
    Container/Span.h
    IO/File.h
    IO/File.cpp
    IO/Logging.h
    IO/Logging.cpp
    IO/Json.h
    IO/Json.cpp
    System/OS.h
    System/OS.cpp
    Math/Math.h
    Math/Math.cpp
    Math/3DLinearAlgebra.h
    Math/3DLinearAlgebra.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_SOURCES})

add_library(rad ${RAD_SOURCES})

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
    PUBLIC WIN32_LEAN_AND_MEAN
)
endif()
