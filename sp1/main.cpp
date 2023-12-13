#include "gtest/gtest.h"
#include "tclap/CmdLine.h"

int main(int argc, char** argv)
{
    try
    {
        TCLAP::CmdLine cmdLine("Test Suite Priority 1");
        cmdLine.parse(argc, argv);
    }
    catch (TCLAP::ArgException& e)
    {
        fprintf(stderr, "TCLAP::ArgException: %s\n", e.error().c_str());
    }

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
