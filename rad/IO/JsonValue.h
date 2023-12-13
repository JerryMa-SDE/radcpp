#pragma once

#include "rad/Core/Global.h"
#include "rad/Core/RefCounted.h"
#include "rad/Core/String.h"
#include "rad/IO/File.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"
#include "rapidjson/stringbuffer.h"

#include <map>

namespace rad
{

using JsonValue = rapidjson::Value;

class JsonValueRef;

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
    using StringRefType = rapidjson::Value::StringRefType;

    JsonValueRef(std::nullptr_t) {}
    JsonValueRef(rapidjson::Value* value) : m_value(value) {}
    JsonValueRef(rapidjson::Value& value) : m_value(&value) {}
    JsonValueRef(const rapidjson::Value* value) :
        m_value(const_cast<rapidjson::Value*>(value)) {}
    JsonValueRef(const rapidjson::Value& value) :
        m_value(const_cast<rapidjson::Value*>(&value)) {}
    ~JsonValueRef() {}

    bool IsValid() const { return (m_value != nullptr); }
    rapidjson::Value& GetRef() { return *m_value; }
    const rapidjson::Value& GetRef() const { return *m_value; }

    operator bool() const { return IsValid(); }
    operator JsonValue& () { return *m_value; }
    operator const JsonValue& () const { return *m_value; }

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
    bool GetBool(bool b = false) const;
    JsonValueRef SetBool(bool b) const { return m_value->SetBool(b); }

    JsonValueRef SetObject() { return m_value->SetObject(); }
    SizeType MemberCount() const { return m_value->MemberCount(); }
    SizeType MemberCapacity() const { return m_value->MemberCapacity(); }
    bool ObjectEmpty() const { return m_value->ObjectEmpty(); }
    JsonValueRef operator[](const char* name)
    {
        if (IsValid() && m_value->IsObject())
        {
            auto iter = m_value->FindMember(name);
            if (iter != m_value->MemberEnd())
            {
                return iter->value;
            }
        }
        return nullptr;
    }
    const JsonValueRef operator[](const char* name) const
    {
        return const_cast<JsonValueRef&>(*this)[name];
    }

    JsonValueRef operator[](const std::string& name)
    {
        return (*this)[name.c_str()];
    }
    const JsonValueRef operator[](const std::string& name) const
    {
        return const_cast<JsonValueRef&>(*this)[name.c_str()];
    }

    ConstMemberIterator MemberBegin() const
    {
        return const_cast<const rapidjson::Value*>(m_value)->MemberBegin();
    }
    ConstMemberIterator MemberEnd() const
    {
        return const_cast<const rapidjson::Value*>(m_value)->MemberEnd();
    }
    MemberIterator MemberBegin() { return m_value->MemberBegin(); }
    MemberIterator MemberEnd() { return m_value->MemberEnd(); }

    template <typename Allocator>
    JsonValueRef MemberReserve(SizeType newCapacity, Allocator& allocator)
    {
        return m_value->MemberReserve(newCapacity, allocator);
    }

    bool HasMember(std::string_view name) const
    {
        return m_value->HasMember(name.data());
    }
    bool HasMemberCaseInsensitive(std::string_view name) const
    {
        return (FindMemberCaseInsensitive(name) != MemberEnd());
    }

    MemberIterator FindMember(std::string_view name)
    {
        return m_value->FindMember(name.data());
    }
    ConstMemberIterator FindMember(std::string_view name) const
    {
        return const_cast<const rapidjson::Value*>(m_value)->FindMember(name.data());
    }

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

    template <typename Allocator>
    JsonValueRef AddMember(std::string_view name, JsonValue& value, Allocator& allocator)
    {
        JsonValue jName;
        jName.SetString(name.data(), static_cast<SizeType>(name.length()), allocator);
        return m_value->AddMember(std::move(jName), value, allocator);
    }

    template <typename Allocator>
    JsonValueRef AddMember(std::string_view name, JsonValue&& value, Allocator& allocator)
    {
        JsonValue jName;
        jName.SetString(name.data(), static_cast<SizeType>(name.length()), allocator);
        return m_value->AddMember(std::move(jName), std::forward<JsonValue>(value), allocator);
    }

    template <typename T, typename Allocator>
    JsonValueRef AddMember(std::string_view name, const T& value, Allocator& allocator)
    {
        JsonValue jName;
        jName.SetString(name.data(), static_cast<SizeType>(name.length()), allocator);
        JsonValue jValue;
        JsonValueRef(jValue).Set(value, allocator);
        return m_value->AddMember(std::move(jName), std::move(jValue), allocator);
    }

    // No copy for value string.
    template <typename Allocator>
    JsonValueRef AddMemberStringRef(std::string_view name, std::string_view value, Allocator& allocator)
    {
        JsonValue jName;
        jName.SetString(name.data(), static_cast<SizeType>(name.length()), allocator);
        JsonValue jValue;
        jValue.SetString(StringRefType(value.data(), static_cast<SizeType>(value.length())));
        return m_value->AddMember(std::move(jName), std::move(jValue), allocator);
    }

    // No copy for name string
    template <typename Allocator>
    JsonValueRef AddMemberNameRef(std::string_view name, JsonValue& value, Allocator& allocator)
    {
        return m_value->AddMember(
            StringRefType(name.data(), static_cast<SizeType>(name.length())),
            value,
            allocator
        );
    }

    // No copy for name string
    template <typename Allocator>
    JsonValueRef AddMemberNameRef(std::string_view name, JsonValue&& value, Allocator& allocator)
    {
        return m_value->AddMember(
            StringRefType(name.data(), static_cast<SizeType>(name.length())),
            std::forward<JsonValue>(value),
            allocator
        );
    }

    // No copy for name string.
    template <typename T, typename Allocator>
    JsonValueRef AddMemberNameRef(std::string_view name, const T& value, Allocator& allocator)
    {
        JsonValue jValue;
        JsonValueRef(jValue).Set(value, allocator);
        return m_value->AddMember(
            StringRefType(name.data(), static_cast<SizeType>(name.length())),
            std::move(jValue),
            allocator
        );
    }

    // No copy for both name and value strings.
    template <typename Allocator>
    JsonValueRef AddMemberNameRefStringRef(std::string_view name, std::string_view value, Allocator& allocator)
    {
        return m_value->AddMember(
            StringRefType(name.data(), static_cast<SizeType>(name.length())),
            StringRefType(value.data(), static_cast<SizeType>(value.length())),
            allocator);
    }

    void RemoveAllMembers() { m_value->RemoveAllMembers(); }
    bool RemoveMember(std::string_view name) { return m_value->RemoveMember(name.data()); }
    MemberIterator RemoveMember(MemberIterator m) { return m_value->RemoveMember(m); }
    MemberIterator EraseMember(ConstMemberIterator pos) { return m_value->EraseMember(pos); }
    MemberIterator EraseMember(ConstMemberIterator first, ConstMemberIterator last)
    {
        return m_value->EraseMember(first, last);
    }

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

    const JsonValueRef operator[](SizeType index) const
    {
        return const_cast<JsonValueRef&>(*this)[index];
    }

    JsonValueRef operator[](int index)
    {
        return (*this)[static_cast<SizeType>(index)];
    }

    const JsonValueRef operator[](int index) const
    {
        return const_cast<JsonValueRef&>(*this)[static_cast<SizeType>(index)];
    }

    ValueIterator ArrayBegin() { return m_value->Begin(); }
    ValueIterator ArrayEnd() { return m_value->End(); }
    ConstValueIterator ArrayBegin() const { return const_cast<const rapidjson::Value*>(m_value)->Begin(); }
    ConstValueIterator ArrayEnd() const { return const_cast<const rapidjson::Value*>(m_value)->End(); }

    template <typename Allocator>
    JsonValueRef ArrayReserve(SizeType newCapacity, Allocator& allocator)
    {
        return m_value->Reserve(newCapacity, allocator);
    }

    template <typename Allocator>
    JsonValueRef ArrayPushBack(JsonValue& value, Allocator& allocator)
    {
        return m_value->PushBack(value, allocator);
    }

    template <typename Allocator>
    JsonValueRef ArrayPushBack(JsonValue&& value, Allocator& allocator)
    {
        return m_value->PushBack(std::forward<JsonValue>(value), allocator);
    }

    template <typename Allocator>
    JsonValueRef ArrayPushBack(StringRefType str, Allocator& allocator)
    {
        return m_value->PushBack(str, allocator);
    }

    template <typename T, typename Allocator>
    JsonValueRef ArrayPushBack(const T& value, Allocator& allocator)
    {
        JsonValue jValue;
        JsonValueRef(jValue).Set(value, allocator);
        return m_value->PushBack(std::move(jValue), allocator);
    }

    template <typename Allocator>
    JsonValueRef ArrayPushBackString(std::string_view str, Allocator& allocator)
    {
        rapidjson::Value val;
        val.SetString(str.data(), static_cast<SizeType>(str.length()), allocator);
        return m_value->PushBack(std::move(val), allocator);
    }

    template <typename Allocator>
    JsonValueRef ArrayPushBackStringRef(std::string_view str, Allocator& allocator)
    {
        return m_value->PushBack(
            StringRefType(str.data(), static_cast<SizeType>(str.length())),
            allocator
        );
    }

    JsonValueRef ArrayPopBack()
    {
        return m_value->PopBack();
    }

    ValueIterator ArrayErase(ConstValueIterator pos)
    {
        return m_value->Erase(pos);
    }

    ValueIterator ArrayErase(ConstValueIterator first, ConstValueIterator last)
    {
        return m_value->Erase(first, last);
    }

    Array GetArray() { return m_value->GetArray(); }
    ConstArray GetArray() const { return const_cast<const rapidjson::Value*>(m_value)->GetArray(); }

    // Support convertions from string (strto*) with base 2(0b), 8(0), 10, 16(0x/0X).
    int GetInt(int i = 0) const;
    uint32_t GetUint(uint32_t u = 0) const;
    int64_t GetInt64(int64_t i64 = 0) const;
    uint64_t GetUint64(int64_t u64 = 0) const;

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

    const char* GetString(const char* str = "") const
    {
        if (IsValid() && m_value->IsString())
        {
            return m_value->GetString();
        }
        else
        {
            return str;
        }
    }

    SizeType GetStringLength() const { return m_value->GetStringLength(); }

    template <typename Allocator>
    JsonValueRef SetString(std::string_view s, Allocator& allocator)
    {
        return m_value->SetString(s.data(), static_cast<SizeType>(s.length()), allocator);
    }

    JsonValueRef SetStringRef(std::string_view s)
    {
        return m_value->SetString(
            StringRefType(s.data(), static_cast<SizeType>(s.length()))
        );
    }

    template <typename T>
    bool Is() const { return m_value->Is<T>(); }

    template <typename T>
    T Get(const T& t = T()) const
    {
        if (IsValid())
        {
            T t = {};
            FromJson(*this, t);
            return t;
        }
        else
        {
            return t;
        }
    }

    template <>
    bool Get(const bool& t) const { return GetBool(); }
    template <>
    int32_t Get(const int32_t& t) const { return GetInt(); }
    template <>
    uint32_t Get(const uint32_t& t) const { return GetUint(); }
    template <>
    int64_t Get(const int64_t& t) const { return GetInt64(); }
    template <>
    uint64_t Get(const uint64_t& t) const { return GetUint64(); }
    template <>
    float Get(const float& t) const { return GetFloat(); }
    template <>
    double Get(const double& t) const { return GetDouble(); }
    template <size_t N>
    const char* Get(const char(&t)[N]) const { return GetString(); }

    template <typename T>
    std::vector<T> GetVector(const T& t = T()) const
    {
        if (IsValid() && IsArray())
        {
            std::vector<T> values;
            values.reserve((ArraySize()));
            for (JsonValueRef jValue : GetArray())
            {
                values.push_back(jValue.Get<T>(t));
            }
            return values;
        }
        return {};
    }

    template<typename T, typename Allocator>
    JsonValueRef Set(const T& data, Allocator& allocator)
    {
        return ToJson(*this, data, allocator);
    }

    JsonValueRef Set(StringRefType data)
    {
        return m_value->SetString(data);
    }

    template<typename Allocator>
    JsonValueRef CreateValueByPointer(std::string_view p, Allocator& allocator)
    {
        return rapidjson::CreateValueByPointer(*m_value, rapidjson::Pointer(p.data()), allocator);
    }

    template<typename T, typename Allocator>
    JsonValueRef SetValueByPointer(std::string_view p, const T& value, Allocator& allocator)
    {
        return rapidjson::SetValueByPointer(*m_value, rapidjson::Pointer(p.data()), value, allocator);
    }

    JsonValueRef GetValueByPointer(std::string_view p)
    {
        return rapidjson::GetValueByPointer(*m_value, rapidjson::Pointer(p.data()));
    }

    template<typename T>
    JsonValueRef GetValueByPointerWithDefault(std::string_view p, const T& value)
    {
        return rapidjson::GetValueByPointerWithDefault(*m_value, rapidjson::Pointer(p.data()), value);
    }

    bool EraseValueByPointer(std::string_view p)
    {
        return rapidjson::EraseValueByPointer(*m_value, rapidjson::Pointer(p.data()));
    }

    rapidjson::StringBuffer Stringify();
    rapidjson::StringBuffer StringifyPretty();

private:
    rapidjson::Value* m_value = nullptr;

}; // class JsonValueRef


void FromJson(const JsonValueRef& json, std::string& str);

template<typename Allocator>
inline JsonValueRef ToJson(JsonValueRef& json, bool val, Allocator& allocator)
{
    return json.SetBool(val);
}

template<typename Allocator>
inline JsonValueRef ToJson(JsonValueRef& json, int32_t val, Allocator& allocator)
{
    return json.SetInt(val);
}

template<typename Allocator>
inline JsonValueRef ToJson(JsonValueRef& json, uint32_t val, Allocator& allocator)
{
    return json.SetUint(val);
}

template<typename Allocator>
inline JsonValueRef ToJson(JsonValueRef& json, int64_t val, Allocator& allocator)
{
    return json.SetInt64(val);
}

template<typename Allocator>
inline JsonValueRef ToJson(JsonValueRef& json, uint64_t val, Allocator& allocator)
{
    return json.SetUint64(val);
}

template<typename Allocator>
inline JsonValueRef ToJson(JsonValueRef& json, float val, Allocator& allocator)
{
    return json.SetFloat(val);
}

template<typename Allocator>
inline JsonValueRef ToJson(JsonValueRef& json, double val, Allocator& allocator)
{
    return json.SetDouble(val);
}

template<typename Allocator>
inline JsonValueRef ToJson(JsonValueRef& json, const char* val, Allocator& allocator)
{
    return json.SetString(val, allocator);
}

template<typename Allocator>
inline JsonValueRef ToJson(JsonValueRef& json, const std::string& val, Allocator& allocator)
{
    return json.SetString(val, allocator);
}

template<typename T, typename Allocator>
inline JsonValueRef ToJson(JsonValueRef& json, const std::vector<T>& vec, Allocator& allocator)
{
    json.SetArray();
    json.ArrayReserve(static_cast<JsonValueRef::SizeType>(vec.size()), allocator);
    for (const auto& t : vec)
    {
        json.ArrayPushBack(t, allocator);
    }
    return json;
}

template<typename T, typename Allocator>
inline JsonValueRef ToJson(JsonValueRef& json, const std::map<std::string, T>& map, Allocator& allocator)
{
    json.SetObject();
    json.MemberReserve(static_cast<JsonValueRef::SizeType>(map.size()), allocator);
    for (const auto& pair : map)
    {
        json.AddMember(pair.first, pair.second, allocator);
    }
    return json;
}

} // namespace rad
