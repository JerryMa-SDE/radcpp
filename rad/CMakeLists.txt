set(RAD_SOURCE_FILES
    Core/Global.h
    Core/Global.cpp
    Core/Memory.h
    Core/Memory.cpp
    Core/Integer.h
    Core/Integer.cpp
    Core/String.h
    Core/String.cpp
    Core/Span.h
    Core/TypeTraits.h
    Core/RefCounted.h
    Core/Flags.h
    Core/Math.h
    Core/Math.cpp
    Core/Numeric.h
    Core/Float16Compressor.h
    IO/File.h
    IO/File.cpp
    IO/Logging.h
    IO/Logging.cpp
    IO/JsonDoc.h
    IO/JsonDoc.cpp
    IO/JsonValue.h
    IO/JsonValue.cpp
    IO/PropertyTree.h
    IO/PropertyTree.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${RAD_SOURCE_FILES})

add_library(rad
    ${RAD_SOURCE_FILES}
)

find_package(Boost)

target_include_directories(rad
    PUBLIC ${Boost_INCLUDE_DIRS}
    PUBLIC ${RADCPP_SOURCE_DIR}/3rdparty/tclap/include/
)

target_link_libraries(rad
    PUBLIC ${Boost_LIBRARIES}
    PUBLIC stb
)

if (WIN32)
target_compile_definitions(rad
    PUBLIC WIN32_LEAN_AND_MEAN
)
endif()
