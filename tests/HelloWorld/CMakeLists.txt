add_executable(HelloWorld
    HelloWorld.cpp
    TestInteger.cpp
)

set_target_properties(HelloWorld PROPERTIES FOLDER "tests")

find_package(GTest CONFIG REQUIRED)
target_link_libraries(HelloWorld
    PRIVATE rad
    PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
)
