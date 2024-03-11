#include <gtest/gtest.h>
#include "rad/IO/Logging.h"

int main(int argc, char* argv[])
{
    rad::SetLogFile("HelloWorld.log", true);

    LogGlobal(Info, "Hello, World!");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
