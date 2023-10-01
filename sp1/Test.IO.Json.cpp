#include "gtest/gtest.h"
#include "rad/IO/Json.h"

TEST(IO, Json)
{
    boost::json::value jRoot = rad::LoadJsonFromFile((const char8_t*)"test.json");
    EXPECT_TRUE(jRoot.is_object());

    rapid::JsonDoc doc;
    bool retParseFile = doc.ParseFile((const char8_t*)"test.json");
    EXPECT_TRUE(retParseFile);
    if (retParseFile)
    {
        rapid::JsonValueRef jRoot = doc.GetRoot();
        rapid::JsonValueRef jPatterns = jRoot["patterns"];
        EXPECT_EQ(jPatterns.ArraySize(), 3);
        EXPECT_EQ(std::string(jPatterns[0]["name"].GetString()), "reset");
        EXPECT_EQ(jPatterns[0]["rows"].GetInt(), 32);
        EXPECT_EQ(std::string(jPatterns[1]["name"].GetString()), "pad sweep intro");
        EXPECT_EQ(jPatterns[1]["rows"].GetInt(), 512);
        EXPECT_EQ(std::string(jPatterns[2]["name"].GetString()), "kick + sample intro");
        EXPECT_EQ(std::string(jPatterns[2]["prop"].GetString("unknown")), "unknown");
        EXPECT_EQ(jPatterns[2]["rows"].GetInt(), 512);
        EXPECT_TRUE(jPatterns[2]["data"].IsArray());
    }
}
