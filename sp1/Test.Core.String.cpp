#include "gtest/gtest.h"
#include "rad/Core/String.h"

TEST(Core, String)
{
    std::string str1 = "a=1; b=12; c=123; d=1234; e=12345; f=123456;";
    auto vecSkipEmpty = rad::StrSplit(str1, ";", true);
    EXPECT_EQ(vecSkipEmpty.size(), 6);
    if (vecSkipEmpty.size() == 6)
    {
        for (std::string& str : vecSkipEmpty)
        {
            rad::StrTrimInPlace(str);
        }
        EXPECT_EQ(vecSkipEmpty[0], "a=1");
        EXPECT_EQ(vecSkipEmpty[1], "b=12");
        EXPECT_EQ(vecSkipEmpty[2], "c=123");
        EXPECT_EQ(vecSkipEmpty[3], "d=1234");
        EXPECT_EQ(vecSkipEmpty[4], "e=12345");
        EXPECT_EQ(vecSkipEmpty[5], "f=123456");
    }

    str1 = "a=1; b=12; c=123; d=1234; e=12345; f=123456;";
    str1 = str1 + " " + str1 + " " + str1;
    std::string str2 = str1;
    str2 = rad::StrReplace(str2, "a=1", "a1");
    str2 = rad::StrReplace(str2, "b=12", "b2");
    str2 = rad::StrReplace(str2, "c=123", "c3");
    str2 = rad::StrReplace(str2, "d=1234", "d4");
    rad::StrReplaceInPlace(str2, "a1", "a=1");
    rad::StrReplaceInPlace(str2, "b2", "b=12");
    rad::StrReplaceInPlace(str2, "c3", "c=123");
    rad::StrReplaceInPlace(str2, "d4", "d=1234");
    rad::StrReplaceInPlace(str2, "a=1", "a1");
    rad::StrReplaceInPlace(str2, "b=12", "b2");
    rad::StrReplaceInPlace(str2, "c=123", "c3");
    rad::StrReplaceInPlace(str2, "d=1234", "d4");
    str2 = rad::StrReplace(str2, "a1", "a=1");
    str2 = rad::StrReplace(str2, "b2", "b=12");
    str2 = rad::StrReplace(str2, "c3", "c=123");
    str2 = rad::StrReplace(str2, "d4", "d=1234");
    EXPECT_EQ(str1, str2);
}
