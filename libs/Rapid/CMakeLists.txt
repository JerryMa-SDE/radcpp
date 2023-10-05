set(Rapid_SOURCE_FILES
    rad/Rapid/JsonValue.cpp
    rad/Rapid/JsonValue.h
    rad/Rapid/JsonDoc.h
    rad/Rapid/JsonDoc.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${Rapid_SOURCE_FILES})

add_library(Rapid
    ${Rapid_SOURCE_FILES}
)

target_include_directories(Rapid
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Rapid
    PUBLIC rad
)
