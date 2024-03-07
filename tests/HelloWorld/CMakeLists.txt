add_executable(HelloWorld
    HelloWorld.cpp
    TestInteger.cpp
    TestFloat.cpp
    TestFlags.cpp
    TestJson.cpp
)

set_target_properties(HelloWorld PROPERTIES FOLDER "tests")

find_package(GTest CONFIG REQUIRED)
target_link_libraries(HelloWorld
    PRIVATE rad
    PRIVATE GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main
)
