#pragma once

#include "rad/Core/Global.h"
#include "JsonValue.h"

namespace rapid
{

using JsonValue = rapidjson::Value;

class JsonDoc : public rad::RefCounted<JsonDoc>
{
public:
    JsonDoc();
    ~JsonDoc();

    bool ParseFile(const rad::FilePath& filePath);
    bool Parse(std::string_view str);
    bool HasParseError() { return m_doc.HasParseError(); }
    const char* GetParseError();
    size_t GetParseErrorOffset();

    rapidjson::Document& GetDoc() { return m_doc; }
    rapidjson::Document::AllocatorType& GetAllocator() { return m_doc.GetAllocator(); }
    JsonValue& GetRoot() { return m_doc.GetObject(); }
    JsonValue& operator[](std::string_view name) { return m_doc[name.data()]; }

    bool IsObject() { return m_doc.IsObject(); }
    bool HasMember(std::string_view name) { return m_doc.HasMember(name.data()); }
    auto MemberBegin() { return m_doc.MemberBegin(); }
    auto MemberEnd() { return m_doc.MemberEnd(); }

    void SetNull() { m_doc.SetNull(); }

    JsonValue& CreateValueByPointer(std::string_view p)
    {
        return rapidjson::CreateValueByPointer(m_doc, rapidjson::Pointer(p.data()));
    }

    template<typename T>
    JsonValue& SetValueByPointer(std::string_view p, T value)
    {
        return rapidjson::SetValueByPointer(m_doc, rapidjson::Pointer(p.data()), value);
    }

    JsonValue* GetValueByPointer(std::string_view p)
    {
        return rapidjson::GetValueByPointer(m_doc, rapidjson::Pointer(p.data()));
    }

    template<typename T>
    JsonValue& GetValueByPointerWithDefault(std::string_view p, T value)
    {
        return rapidjson::GetValueByPointerWithDefault(m_doc, rapidjson::Pointer(p.data()), value);
    }

    std::string Stringify();
    std::string StringifyPretty();

private:
    rapidjson::Document m_doc;

}; // class JsonDoc

} // namespace rapid
