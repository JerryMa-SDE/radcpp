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
    Core/Logging.h
    Core/Logging.cpp
    Core/OS.h
    Core/OS.cpp
    Core/File.h
    Core/File.cpp
    Core/Flags.h
    Core/Span.h
    Core/RefCounted.h
    Core/Memory.h
    Core/Memory.cpp
    Core/Math.h
    Core/Math.cpp
    Core/3D.h
    Core/3D.cpp
    Core/TypeTraits.h
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_SOURCES})

add_library(rad ${RAD_SOURCES})

find_package(Boost)
find_package(spdlog CONFIG REQUIRED)

target_include_directories(rad
    PUBLIC ${Boost_INCLUDE_DIRS}
)

target_link_libraries(rad
    PUBLIC ${Boost_LIBRARIES}
    PUBLIC stb
    PUBLIC spdlog::spdlog
)

if (WIN32)
target_compile_definitions(rad
    PUBLIC WIN32_LEAN_AND_MEAN
)
endif()
