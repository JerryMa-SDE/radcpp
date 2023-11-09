#pragma once

#include "rad/Core/Global.h"
#include "JsonValue.h"

namespace rad
{

using JsonValue = rapidjson::Value;

// wrapper of rapidjson::Document (UTF8, with default allocator).
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
    const rapidjson::Document& GetDoc() const { return m_doc; }
    operator rapidjson::Document& () { return m_doc; }
    operator const rapidjson::Document& () const { return m_doc; }
    rapidjson::Document::AllocatorType& GetAllocator() { return m_doc.GetAllocator(); }
    JsonValueRef GetRoot();
    const JsonValueRef GetRoot() const { return const_cast<JsonDoc&>(*this).GetRoot(); }
    JsonValueRef operator[](std::string_view name) { return GetRoot()[name.data()]; }
    const JsonValueRef operator[](std::string_view name) const { return GetRoot()[name.data()]; }

    bool IsObject() { return m_doc.IsObject(); }
    bool HasMember(std::string_view name) { return m_doc.HasMember(name.data()); }
    auto MemberBegin() { return m_doc.MemberBegin(); }
    auto MemberEnd() { return m_doc.MemberEnd(); }

    void SetNull() { m_doc.SetNull(); }

    JsonValueRef CreateValueByPointer(std::string_view p)
    {
        return rapidjson::CreateValueByPointer(m_doc, rapidjson::Pointer(p.data()));
    }

    template<typename T>
    JsonValueRef SetValueByPointer(std::string_view p, const T& value)
    {
        return rapidjson::SetValueByPointer(m_doc, rapidjson::Pointer(p.data()), value);
    }

    JsonValueRef GetValueByPointer(std::string_view p)
    {
        return rapidjson::GetValueByPointer(m_doc, rapidjson::Pointer(p.data()));
    }

    template<typename T>
    JsonValueRef GetValueByPointerWithDefault(std::string_view p, const T& value)
    {
        return rapidjson::GetValueByPointerWithDefault(m_doc, rapidjson::Pointer(p.data()), value);
    }

    bool EraseValueByPointer(std::string_view p)
    {
        return rapidjson::EraseValueByPointer(m_doc, rapidjson::Pointer(p.data()));
    }

    rapidjson::StringBuffer Stringify();
    rapidjson::StringBuffer StringifyPretty();

private:
    rapidjson::Document m_doc;

}; // class JsonDoc

} // namespace rad
