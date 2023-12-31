#include "gtest/gtest.h"
#include "rad/IO/JsonDoc.h"
#include "rad/IO/JsonValue.h"

struct MenuItem
{
    std::string value;
    std::string onClick;
};

void FromJson(const rad::JsonValueRef json, MenuItem& item)
{
    item.value = json["value"].GetString("");
    item.onClick = json["onclick"].GetString("");
}

template<typename Allocator>
rad::JsonValueRef ToJson(rad::JsonValueRef json, const MenuItem& item, Allocator& allocator)
{
    json.SetObject();
    json.AddMemberNameRef("value", item.value, allocator);
    json.AddMemberNameRef("onclick", item.onClick, allocator);
    return json;
}

TEST(IO, Json)
{
    rad::Ref<rad::JsonDoc> doc = RAD_NEW rad::JsonDoc();
    if (doc->ParseFile("data/menu.json"))
    {
        EXPECT_TRUE(doc->GetRoot().IsObject());
        rad::JsonValueRef jMenuItems = doc->GetRoot()["menu"]["popup"]["menuitem"];
        if (jMenuItems)
        {
            EXPECT_TRUE(jMenuItems.IsArray());
            EXPECT_EQ(jMenuItems.ArraySize(), 3);

            MenuItem helpItem = {};
            helpItem.value = "Help";
            helpItem.onClick = "ShowHelp()";
            jMenuItems.ArrayPushBack(helpItem, doc->GetAllocator());

            auto items = jMenuItems.GetVector<MenuItem>();
            EXPECT_EQ(items.size(), 4);
            if (items.size() == 4)
            {
                EXPECT_EQ(items[0].value, "New");
                EXPECT_EQ(items[0].onClick, "CreateNewDoc()");
                EXPECT_EQ(items[1].value, "Open");
                EXPECT_EQ(items[1].onClick, "OpenDoc()");
                EXPECT_EQ(items[2].value, "Close");
                EXPECT_EQ(items[2].onClick, "CloseDoc()");
                EXPECT_EQ(items[3].value, helpItem.value);
                EXPECT_EQ(items[3].onClick, helpItem.onClick);
            }
        }
        doc->SaveToFile("data/menu-modified.json");
    }
    EXPECT_TRUE(rad::Exists(rad::FilePath("data/menu-modified.json")));
}
