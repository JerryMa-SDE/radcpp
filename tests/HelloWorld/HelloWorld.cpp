#include <gtest/gtest.h>
#include "rad/Core/Logging.h"

int main(int argc, char* argv[])
{
    LogGlobal(Info, "Hello, World!");
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
