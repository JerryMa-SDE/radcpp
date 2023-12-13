#include "gtest/gtest.h"
#include "rad/IO/File.h"

TEST(IO, File)
{
    rad::File file;
    rad::FilePath path = (char8_t*)"Test.IO.File.txt";
    file.Open(path, rad::FileAccessWrite);
    EXPECT_TRUE(file.IsOpen());
    const uint32_t lineCount = 32;
    const char* pHelloWorld = "Hello, World!";
    if (file.IsOpen())
    {
        for (uint32_t i = 0; i < lineCount; ++i)
        {
            file.Print("%s\n", pHelloWorld);
        }
        file.Close();
    }

    file.Open(path, rad::FileAccessRead);
    EXPECT_TRUE(file.IsOpen());
    if (file.IsOpen())
    {
        auto lines = rad::File::ReadLines(path);
        EXPECT_EQ(lines.size(), lineCount + 1);
        if (lines.size() == lineCount + 1)
        {
            for (uint32_t i = 0; i < lineCount; ++i)
            {
                EXPECT_EQ(lines[i], pHelloWorld);
            }
            // the last line is empty (line feed).
            EXPECT_TRUE(lines[lineCount].empty());
        }
        file.Close();
    }
    EXPECT_TRUE(rad::Remove(path));
    EXPECT_FALSE(rad::Exists(path));
}
