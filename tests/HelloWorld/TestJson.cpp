#include <gtest/gtest.h>
#include <boost/version.hpp>
#include "rad/IO/Json.h"

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
    TestValueConversion();
}
