#include "gtest/gtest.h"
#include "rad/IO/File.h"

TEST(IO, File)
{
    rad::File file;
    rad::FilePath path = (char8_t*)"rad.txt";
    file.Open(path, rad::FileAccessWrite);
    EXPECT_TRUE(file.IsOpen());
    for (uint32_t i = 0; i < 32; ++i)
    {
        file.Print("Line %2u: Hello, World!\n", i + 1);
    }
    file.Flush();
    file.Close();
    file.Open(path, rad::FileAccessRead);
    EXPECT_TRUE(file.IsOpen());
    auto lines = rad::File::ReadLines(path);
    EXPECT_EQ(lines.size(), 33);
    if (lines.size() == 33)
    {
        for (uint32_t i = 0; i < 32; ++i)
        {
            EXPECT_EQ(lines[i], rad::StrPrint("Line %2u: Hello, World!", i + 1));
        }
        EXPECT_TRUE(lines[32].empty());
    }
    file.Close();
    rad::Remove(path);
    EXPECT_FALSE(rad::Exists(path));
}
