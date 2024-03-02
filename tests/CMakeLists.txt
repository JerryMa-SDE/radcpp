add_executable(HelloWorld
    HelloWorld.cpp
)

set_target_properties(HelloWorld PROPERTIES FOLDER "tests")
target_link_libraries(HelloWorld
    rad
)
