#include <gtest/gtest.h>
#include "rad/Core/String.h"
#include "rad/IO/Json.h"
#include "rad/IO/Logging.h"

void TestParsing()
{
    using namespace boost::json;
    value jRoot = rad::ParseJsonFromFile("prize.json");
    EXPECT_TRUE(jRoot.is_object());
    if (jRoot.is_object())
    {
        EXPECT_TRUE(jRoot.as_object()["prizes"].is_array());
    }
}

void TestValueConversion()
{
    using namespace boost::json;

    std::vector<int> v1{ 1, 2, 3, 4 };
    value jv = value_from(v1);
    EXPECT_EQ(serialize(jv), "[1,2,3,4]");

    std::vector<int> v2 = value_to<std::vector<int>>(jv);
    EXPECT_EQ(v1, v2);
}

TEST(Core, Json)
{
    TestParsing();
    TestValueConversion();
}
