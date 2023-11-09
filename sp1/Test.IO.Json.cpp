#include "gtest/gtest.h"
#include "rad/IO/File.h"
#include "rad/IO/JsonDoc.h"
#include "rad/IO/JsonValue.h"

using namespace rad;

TEST(IO, Json)
{
    Ref<rad::JsonDoc> doc = RAD_NEW JsonDoc();
    EXPECT_TRUE(doc->ParseFile("data/glossary.json"));
    JsonValueRef jGlossEntry = doc->GetValueByPointer("/glossary/GlossDiv/GlossList/GlossEntry");
    EXPECT_TRUE(jGlossEntry);
    if (jGlossEntry)
    {
        EXPECT_STREQ(jGlossEntry["ID"].GetString(), "SGML");
        EXPECT_STREQ(jGlossEntry["Abbrev"].GetString(), "ISO 8879:1986");
        JsonValueRef jGlossDef = jGlossEntry["GlossDef"];
        EXPECT_TRUE(jGlossDef["GlossSeeAlso"].IsArray());
        auto seeAlso = jGlossDef["GlossSeeAlso"].GetVector<const char*>();
        EXPECT_EQ(seeAlso.size(), 2);
        if (seeAlso.size() == 2)
        {
            EXPECT_STREQ(seeAlso[0], "GML");
            EXPECT_STREQ(seeAlso[1], "XML");
        }
    }
    EXPECT_TRUE(doc->ParseFile("data/menu.json"));
    EXPECT_TRUE(doc->GetRoot().IsObject());
    JsonValueRef jmenuitem = doc->GetRoot()["menu"]["popup"]["menuitem"];
    EXPECT_TRUE(jmenuitem);
    if (jmenuitem)
    {
        EXPECT_TRUE(jmenuitem.IsArray());
        EXPECT_EQ(jmenuitem.ArraySize(), 3);
    }
    EXPECT_TRUE(doc->ParseFile("data/widget.json"));
    EXPECT_TRUE(doc->ParseFile("data/web-app.json"));
    JsonValueRef jServlet = doc->GetRoot()["web-app"]["servlet"];
    JsonValueRef jTrack = doc->SetValueByPointer("/web-app/servlet/0/cachePackageTagsTrack", "0b100000000");
    JsonValueRef jStore = doc->SetValueByPointer("/web-app/servlet/0/cachePackageTagsStore", "0x100");
    JsonValueRef jRefresh = doc->SetValueByPointer("/web-app/servlet/0/cachePackageTagsRefresh", 90);
    if (jServlet && jServlet.IsArray())
    {
        EXPECT_EQ(jTrack.GetUint(200), 0x100);
        EXPECT_EQ(jStore.GetUint(200), 0x100);
        EXPECT_EQ(jRefresh.GetUint(60), 90);
    }
    EXPECT_TRUE(doc->ParseFile("data/menu-items-null.json"));
}
