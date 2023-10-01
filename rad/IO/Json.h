#pragma once

#include "rad/Core/Global.h"
#include "rad/Core/String.h"
#include "rad/Core/RefCounted.h"
#include "rad/IO/File.h"

#include "boost/json.hpp"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"

namespace rad {

boost::json::value LoadJsonFromFile(const FilePath& path);

} // namespace rad

namespace rapid {

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

// [Experimental] A wrapper for rapidjson::Value to simplify its usage.
class JsonValueRef
{
public:
    using SizeType = rapidjson::SizeType;
    using MemberIterator = rapidjson::Value::MemberIterator;
    using ConstMemberIterator = rapidjson::Value::ConstMemberIterator;
    using ValueIterator = rapidjson::Value::ValueIterator;
    using ConstValueIterator = rapidjson::Value::ConstValueIterator;
    using Array = rapidjson::Value::Array;
    using ConstArray = rapidjson::Value::ConstArray;

    JsonValueRef(std::nullptr_t) {}
    JsonValueRef(rapidjson::Value& value) : m_value(&value) {}
    ~JsonValueRef() {}

    bool IsValid() const { return (m_value != nullptr); }
    rapidjson::Value* GetValue() { return m_value; }

    bool IsNull()   const { return m_value->IsNull(); }
    bool IsFalse()  const { return m_value->IsFalse(); }
    bool IsTrue()   const { return m_value->IsTrue(); }
    bool IsBool()   const { return m_value->IsBool(); }
    bool IsObject() const { return m_value->IsObject(); }
    bool IsArray()  const { return m_value->IsArray(); }
    bool IsNumber() const { return m_value->IsNumber(); }
    bool IsInt()    const { return m_value->IsInt(); }
    bool IsUint()   const { return m_value->IsUint(); }
    bool IsInt64()  const { return m_value->IsInt64(); }
    bool IsUint64() const { return m_value->IsUint64(); }
    bool IsDouble() const { return m_value->IsDouble(); }
    bool IsString() const { return m_value->IsString(); }
    bool IsLosslessDouble() const { return m_value->IsLosslessDouble(); }
    bool IsFloat() const { return m_value->IsFloat(); }
    bool IsLosslessFloat() const { return m_value->IsLosslessFloat(); }

    JsonValueRef SetNull() const { m_value->SetNull(); }
    bool GetBool() const { m_value->GetBool(); }
    JsonValueRef SetBool(bool b) const { m_value->SetBool(b); }

    JsonValueRef SetObject() { return m_value->SetObject(); }
    SizeType MemberCount() const { return m_value->MemberCount(); }
    SizeType MemberCapacity() const { return m_value->MemberCapacity(); }
    bool ObjectEmpty() const { return m_value->ObjectEmpty(); }
    JsonValueRef operator[](std::string_view name)
    {
        if (IsValid() && m_value->IsObject())
        {
            auto iter = m_value->FindMember(name.data());
            if (iter != m_value->MemberEnd())
            {
                return iter->value;
            }
        }
        return nullptr;
    }
    const JsonValueRef operator[](std::string_view name) const { return const_cast<JsonValueRef&>(*this)[name]; }

    ConstMemberIterator MemberBegin() const { return const_cast<const rapidjson::Value*>(m_value)->MemberBegin(); }
    ConstMemberIterator MemberEnd() const { return const_cast<const rapidjson::Value*>(m_value)->MemberEnd(); }
    MemberIterator MemberBegin() { return m_value->MemberBegin(); }
    MemberIterator MemberEnd() { return m_value->MemberEnd(); }
    JsonValueRef MemberReserve(SizeType newCapacity, JsonDoc* doc) { m_value->MemberReserve(newCapacity, doc->GetAllocator()); }

    bool HasMember(std::string_view name) const { return m_value->HasMember(name.data()); }
    bool HasMemberCaseInsensitive(std::string_view name) const
    {
        return (FindMemberCaseInsensitive(name) != MemberEnd());
    }
    MemberIterator FindMember(std::string_view name) { return m_value->FindMember(name.data()); }
    ConstMemberIterator FindMember(std::string_view name) const { return const_cast<const rapidjson::Value*>(m_value)->FindMember(name.data()); }
    MemberIterator FindMemberCaseInsensitive(std::string_view name)
    {
        if (m_value->IsObject())
        {
            for (auto iter = m_value->MemberBegin(); iter != m_value->MemberEnd(); iter++)
            {
                if (rad::StrCaseEqual(name, iter->name.GetString()))
                {
                    return iter;
                }
            }
        }
        return m_value->MemberEnd();
    }

    ConstMemberIterator FindMemberCaseInsensitive(std::string_view name) const
    {
        return const_cast<JsonValueRef&>(*this).FindMemberCaseInsensitive(name);
    }

    template <typename T>
    JsonValueRef AddMember(std::string_view name, T value, JsonDoc* doc)
    {
        return m_value->AddMember(name.data(), value, doc->GetAllocator());
    }
    void RemoveAllMembers() { m_value->RemoveAllMembers(); }
    bool RemoveMember(std::string_view name) { return m_value->RemoveMember(name.data()); }
    MemberIterator RemoveMember(MemberIterator m) { return m_value->RemoveMember(m); }
    MemberIterator EraseMember(ConstMemberIterator pos) { return m_value->EraseMember(pos); }
    MemberIterator EraseMember(ConstMemberIterator first, ConstMemberIterator last) { return m_value->EraseMember(first, last); }
    bool EraseMember(const std::string_view name) { return m_value->EraseMember(name.data()); }

    JsonValueRef SetArray() { return m_value->SetArray(); }
    SizeType ArraySize() const { return m_value->Size(); }
    SizeType ArrayCapacity() const { return m_value->Capacity(); }
    bool ArrayEmpty() const { return m_value->Empty(); }
    void ArrayClear() { return m_value->Clear(); }
    JsonValueRef operator[](SizeType index)
    {
        if (IsValid() && IsArray() && index < ArraySize())
        {
            return (*m_value)[index];
        }
        else
        {
            return nullptr;
        }
    }
    const JsonValueRef operator[](SizeType index) const { return const_cast<JsonValueRef&>(*this)[index]; }
    ValueIterator ArrayBegin() { return m_value->Begin(); }
    ValueIterator ArrayEnd() { return m_value->End(); }
    ConstValueIterator ArrayBegin() const { return const_cast<const rapidjson::Value*>(m_value)->Begin(); }
    ConstValueIterator ArrayEnd() const { return const_cast<const rapidjson::Value*>(m_value)->End(); }
    JsonValueRef ArrayReserve(SizeType newCapacity, JsonDoc* doc) { m_value->Reserve(newCapacity, doc->GetAllocator()); }
    template <typename T>
    JsonValueRef ArrayPushBack(T value, JsonDoc* doc)
    {
        return m_value->PushBack(value, doc->GetAllocator());
    }

    template <>
    JsonValueRef ArrayPushBack(std::string_view s, JsonDoc* doc)
    {
        rapidjson::Value val;
        val.SetString(s.data(), static_cast<SizeType>(s.length()), doc->GetAllocator());
        return m_value->PushBack(val.Move(), doc->GetAllocator());
    }

    JsonValueRef ArrayPopBack()
    {
        return m_value->PopBack();
    }

    ValueIterator ArrayErase(ConstValueIterator pos) {
        return m_value->Erase(pos);
    }

    ValueIterator ArrayErase(ConstValueIterator first, ConstValueIterator last)
    {
        return m_value->Erase(first, last);
    }

    Array GetArray() { return m_value->GetArray(); }
    ConstArray GetArray() const { return const_cast<const rapidjson::Value*>(m_value)->GetArray(); }

    std::vector<JsonValueRef> GetArrayValues()
    {
        std::vector<JsonValueRef> arr;
        arr.reserve(m_value->Size());
        for (rapidjson::Value& value : m_value->GetArray())
        {
            arr.push_back(JsonValueRef(value));
        }
        return arr;
    }

    int GetInt(int i = 0) const
    {
        if (IsValid() && IsInt())
        {
            return m_value->GetInt();
        }
        else
        {
            return i;
        }
    }

    unsigned GetUint(unsigned u = 0) const
    {
        if (IsValid() && IsUint())
        {
            return m_value->GetUint();
        }
        else
        {
            return u;
        }
    }

    int64_t GetInt64(int64_t i64 = 0) const
    {
        if (IsValid() && IsInt64())
        {
            return m_value->GetInt64();
        }
        else
        {
            return i64;
        }
    }

    uint64_t GetUint64(int64_t u64 = 0) const
    {
        if (IsValid() && IsUint64())
        {
            return m_value->GetUint64();
        }
        else
        {
            return u64;
        }
    }

    double GetDouble(double d = 0.0) const
    {
        if (IsValid() && IsDouble())
        {
            return m_value->GetDouble();
        }
        else
        {
            return d;
        }
    }

    float GetFloat(float f = 0.0f) const
    {
        if (IsValid() && IsFloat())
        {
            return m_value->GetFloat();
        }
        else
        {
            return f;
        }
    }

    JsonValueRef SetInt(int i) { return m_value->SetInt(i); }
    JsonValueRef SetUint(unsigned u) { return m_value->SetUint(u); }
    JsonValueRef SetInt64(int64_t i64) { return m_value->SetInt64(i64); }
    JsonValueRef SetUint64(uint64_t u64) { return m_value->SetUint64(u64); }
    JsonValueRef SetDouble(double d) { return m_value->SetDouble(d); }
    JsonValueRef SetFloat(float f) { return m_value->SetFloat(f); }

    const char* GetString() const { m_value->GetString(); }
    SizeType GetStringLength() const { m_value->GetStringLength(); }
    JsonValueRef SetString(std::string_view s, JsonDoc* doc) { return m_value->SetString(s.data(), static_cast<SizeType>(s.length()), doc->GetAllocator()); }

    template <typename T>
    bool Is() const { return m_value->Is<T>(); }
    template <typename T>
    T Get() const { return m_value->Get<T>(); }
    template <typename T>
    T Get() { return m_value->Get<T>(); }

    template<typename T>
    JsonValueRef Set(const T& data, JsonDoc* doc) { return m_value->Set<T>(data, doc->GetAllocator()); }

private:
    rapidjson::Value* m_value = nullptr;

}; // class JsonValueRef

} // namespace rapid
