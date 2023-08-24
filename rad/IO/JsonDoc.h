#pragma once

#include "rad/Core/Global.h"
#include "rad/Core/String.h"
#include "rad/Core/RefCounted.h"
#include "rad/IO/File.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

RAD_BEGIN_NAMESPACE

using JsonValue = rapidjson::Value;

class JsonDoc : public RefCounted<JsonDoc>
{
public:
    JsonDoc();
    ~JsonDoc();

    bool ParseFile(const FilePath& filePath);
    bool Parse(std::string_view str);
    bool HasParseError() { return m_doc.HasParseError(); }
    const char* GetParseError();
    size_t GetParseErrorOffset();

    rapidjson::Document& GetDoc() { return m_doc; }
    JsonValue& GetRoot() { return m_doc.GetObject(); }
    JsonValue& operator[](std::string_view name) { return m_doc[name.data()]; }

    bool IsObject() { return m_doc.IsObject(); }
    bool HasMember(std::string_view name) { return m_doc.HasMember(name.data()); }
    auto MemberBegin() { return m_doc.MemberBegin(); }
    auto MemberEnd() { return m_doc.MemberEnd(); }

    void SetNull() { m_doc.SetNull(); }

    std::string Stringify();
    std::string StringifyPretty();

private:
    rapidjson::Document m_doc;

}; // class JsonDoc

RAD_END_NAMESPACE
