set(TEST_SOURCE_FILES
    main.cpp
    Core/FlagsTest.cpp
    Core/FloatTest.cpp
    Core/IntegerTest.cpp
    Core/SpanTest.cpp
    IO/JsonTest.cpp
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${TEST_SOURCE_FILES})

add_executable(test ${TEST_SOURCE_FILES})

find_package(GTest CONFIG REQUIRED)
target_link_libraries(test
    PRIVATE rad
    PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
)

add_custom_command(TARGET test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/data/
        ${CMAKE_CURRENT_BINARY_DIR}/data/
)
