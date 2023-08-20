#include "gtest/gtest.h"
#include "rad/Core/String.h"

TEST(Core, String)
{
    std::string str = "a = 1;  b = 2;;    c = 3;;;    d = 4    ;";
    auto vec = rad::StrSplit(str, ";", false);
    EXPECT_EQ(vec.size(), 8);
    EXPECT_TRUE(vec[2].empty());
    EXPECT_TRUE(vec[4].empty());
    EXPECT_TRUE(vec[5].empty());
    EXPECT_TRUE(vec[7].empty());
    auto vecSkipEmpty = rad::StrSplit(str, ";", true);
    EXPECT_EQ(vecSkipEmpty.size(), 4);
    EXPECT_EQ(rad::StrTrim(vecSkipEmpty[0]), "a = 1");
    EXPECT_EQ(rad::StrTrim(vecSkipEmpty[1]), "b = 2");
    EXPECT_EQ(rad::StrTrim(vecSkipEmpty[2]), "c = 3");
    EXPECT_EQ(rad::StrTrim(vecSkipEmpty[3]), "d = 4");
    rad::StrTrimInPlace(vecSkipEmpty[0]);
    rad::StrTrimInPlace(vecSkipEmpty[1]);
    rad::StrTrimInPlace(vecSkipEmpty[2]);
    rad::StrTrimInPlace(vecSkipEmpty[3]);
    EXPECT_EQ(vecSkipEmpty[0], "a = 1");
    EXPECT_EQ(vecSkipEmpty[1], "b = 2");
    EXPECT_EQ(vecSkipEmpty[2], "c = 3");
    EXPECT_EQ(vecSkipEmpty[3], "d = 4");

    str = "a = 1; b = 2; c = 3; d = 4;";
    std::string str1 = str + " " + str;
    std::string str2 = str1;
    str2 = rad::StrReplace(str2, "a = 1", "a1");
    str2 = rad::StrReplace(str2, "b = 2", "b2");
    str2 = rad::StrReplace(str2, "c = 3", "c3");
    str2 = rad::StrReplace(str2, "d = 4", "d4");
    rad::StrReplaceInPlace(str2, "a1", "a = 1");
    rad::StrReplaceInPlace(str2, "b2", "b = 2");
    rad::StrReplaceInPlace(str2, "c3", "c = 3");
    rad::StrReplaceInPlace(str2, "d4", "d = 4");
    rad::StrReplaceInPlace(str2, "a = 1", "a1");
    rad::StrReplaceInPlace(str2, "b = 2", "b2");
    rad::StrReplaceInPlace(str2, "c = 3", "c3");
    rad::StrReplaceInPlace(str2, "d = 4", "d4");
    str2 = rad::StrReplace(str2, "a1", "a = 1");
    str2 = rad::StrReplace(str2, "b2", "b = 2");
    str2 = rad::StrReplace(str2, "c3", "c = 3");
    str2 = rad::StrReplace(str2, "d4", "d = 4");
    EXPECT_EQ(str1, str2);
}
