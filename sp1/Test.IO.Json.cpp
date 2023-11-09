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
    JsonValueRef jMenuItem = doc->GetRoot()["menu"]["popup"]["menuitem"];
    EXPECT_TRUE(jMenuItem);
    if (jMenuItem)
    {
        EXPECT_TRUE(jMenuItem.IsArray());
        EXPECT_EQ(jMenuItem.ArraySize(), 3);
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
        EXPECT_EQ(jInitParam["cachePackageTagsTrack"].GetUint(), 0b100000000);
        EXPECT_EQ(jInitParam["cachePackageTagsStore"].GetUint(), 0x100);
        EXPECT_EQ(jInitParam["cachePackageTagsRefresh"].GetUint(), 90);
    }
    EXPECT_TRUE(doc->ParseFile("data/menu-items-null.json"));
}
