#include "gtest/gtest.h"
#include "rad/IO/File.h"
#include "rad/IO/JsonDoc.h"
#include "rad/IO/JsonValue.h"

using namespace rad;

struct MenuItem
{
    std::string value;
    std::string onClick;
};

void FromJson(const JsonValueRef json, MenuItem& item)
{
    item.value = json["value"].GetString("Unknown");
    item.onClick = json["onclick"].GetString("Undefined");
}

template<typename Allocator>
JsonValueRef ToJson(JsonValueRef json, const MenuItem& item, Allocator& allocator)
{
    json.SetObject();
    json.AddMemberNameRef("value", item.value, allocator);
    json.AddMemberNameRef("onclick", item.onClick, allocator);
    return json;
}

TEST(IO, Json)
{
    Ref<JsonDoc> doc = RAD_NEW JsonDoc();
    EXPECT_TRUE(doc->ParseFile("data/glossary.json"));
    JsonValueRef jGlossEntry = doc->GetValueByPointer("/glossary/GlossDiv/GlossList/GlossEntry");
    EXPECT_TRUE(jGlossEntry);
    if (jGlossEntry)
    {
        EXPECT_STREQ(jGlossEntry["ID"].GetString(), "SGML");
        EXPECT_STREQ(jGlossEntry["Abbrev"].GetString(), "ISO 8879:1986");
        JsonValueRef jGlossDef = jGlossEntry["GlossDef"];
        EXPECT_TRUE(jGlossDef["GlossSeeAlso"].IsArray());
        auto seeAlso = jGlossDef["GlossSeeAlso"].GetVector<std::string>();
        EXPECT_EQ(seeAlso.size(), 2);
        if (seeAlso.size() == 2)
        {
            EXPECT_EQ(seeAlso[0], "GML");
            EXPECT_EQ(seeAlso[1], "XML");
        }
    }
    EXPECT_TRUE(doc->ParseFile("data/menu.json"));
    EXPECT_TRUE(doc->GetRoot().IsObject());
    JsonValueRef jMenuItems = doc->GetRoot()["menu"]["popup"]["menuitem"];
    EXPECT_TRUE(jMenuItems);
    if (jMenuItems)
    {
        EXPECT_TRUE(jMenuItems.IsArray());
        EXPECT_EQ(jMenuItems.ArraySize(), 3);
        MenuItem helpItem = {};
        helpItem.value = "Help";
        helpItem.onClick = "ViewHelp()";
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
    EXPECT_TRUE(doc->ParseFile("data/widget.json"));
    EXPECT_TRUE(doc->ParseFile("data/web-app.json"));
    JsonValueRef jServlet0 = doc->GetRoot()["web-app"]["servlet"][0];
    JsonValueRef jTrack = doc->SetValueByPointer("/web-app/servlet/0/init-param/cachePackageTagsTrack", "0b100000000");
    JsonValueRef jStore = doc->SetValueByPointer("/web-app/servlet/0/init-param/cachePackageTagsStore", "0x100");
    JsonValueRef jRefresh = doc->SetValueByPointer("/web-app/servlet/0/init-param/cachePackageTagsRefresh", 90);
    if (jServlet0 && jServlet0.IsObject())
    {
        JsonValueRef jInitParam = jServlet0["init-param"];
        JsonValueRef jAdminEmail = jInitParam["configGlossary:adminEmail"];
        EXPECT_STREQ(jAdminEmail.GetString(), "ksm@pobox.com");
        std::string mail = "majunlichn@outlook.com";
        jAdminEmail.Set(mail, doc->GetAllocator());
        jAdminEmail.Set("majunlichn@outlook.com"); // StringRefType
        EXPECT_EQ(jAdminEmail.GetString(), mail);
        EXPECT_EQ(jInitParam["cachePackageTagsTrack"].GetUint(), 0b100000000);
        EXPECT_EQ(jInitParam["cachePackageTagsStore"].GetUint(), 0x100);
        EXPECT_EQ(jInitParam["cachePackageTagsRefresh"].GetUint(), 90);
        JsonValue jNumbers;
        std::vector<int> numbers = { 1, 2, 3, 4 };
        JsonValueRef(jNumbers).Set(numbers, doc->GetAllocator());
        JsonValueRef(jNumbers).ArrayPushBack(5, doc->GetAllocator());
        JsonValueRef(jNumbers).ArrayPushBack(JsonValue(6), doc->GetAllocator());
        JsonValueRef(jNumbers).ArrayPushBackString("0b0111", doc->GetAllocator()); // 7
        JsonValueRef(jNumbers).ArrayPushBackStringRef("0x8", doc->GetAllocator()); // 8
        jInitParam.AddMember("Numbers", std::move(jNumbers), doc->GetAllocator());
        int sum = 0;
        for (auto number : jInitParam["Numbers"].GetVector(0))
        {
            sum += number;
        }
        EXPECT_EQ(sum, 36);
        jInitParam["Numbers"][0].SetInt(0);
        EXPECT_TRUE(jInitParam.RemoveMember("Numbers"));
    }
    EXPECT_TRUE(doc->ParseFile("data/menu-items-null.json"));

    std::map<std::string, int> map =
    {
        { "a", 0 },
        { "b", 1 },
        { "c", 2 },
    };
    JsonValue j;
    JsonValueRef(j).Set(map, doc->GetAllocator());
    EXPECT_EQ(j["a"], 0);
    EXPECT_EQ(j["b"], 1);
    EXPECT_EQ(j["c"], 2);
}
