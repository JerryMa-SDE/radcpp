#pragma once

#include "rad/Core/Global.h"
#include "rad/Core/String.h"
#include "rad/Core/RefCounted.h"
#include "rad/IO/File.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"
#include "rapidjson/pointer.h"

namespace rapid
{

using JsonValue = rapidjson::Value;

class JsonValueRef;

template<typename T>
T FromJson(const JsonValueRef& json)
{
    assert(false && "not implemented!");
    return T();
}

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
    JsonValueRef(rapidjson::Value* value) : m_value(value) {}
    JsonValueRef(rapidjson::Value& value) : m_value(&value) {}
    ~JsonValueRef() {}

    bool IsValid() const { return (m_value != nullptr); }
    rapidjson::Value* GetValue() { return m_value; }

    operator bool() const { return IsValid(); }

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
    bool GetBool() const { return m_value->GetBool(); }
    JsonValueRef SetBool(bool b) const { m_value->SetBool(b); }

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
        m_value->MemberReserve(newCapacity, allocator);
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

    template <typename T, typename Allocator>
    JsonValueRef AddMember(std::string_view name, T value, Allocator& allocator)
    {
        return m_value->AddMember(name.data(), value, allocator);
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

    template<typename IndexType,
        typename = std::enable_if_t<std::is_integral_v<IndexType> && !std::is_same_v<IndexType, SizeType>>>
    JsonValueRef operator[](IndexType index)
    {
        return (*this)[static_cast<SizeType>(index)];
    }
    template<typename IndexType,
        typename = std::enable_if_t<std::is_integral_v<IndexType> && !std::is_same_v<IndexType, SizeType>>>
    const JsonValueRef operator[](IndexType index) const
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
        m_value->Reserve(newCapacity, allocator);
    }

    template <typename T, typename Allocator>
    JsonValueRef ArrayPushBack(T value, Allocator& allocator)
    {
        return m_value->PushBack(value, allocator);
    }

    template <typename Allocator>
    JsonValueRef ArrayPushBack(std::string_view s, Allocator& allocator)
    {
        rapidjson::Value val;
        val.SetString(s.data(), static_cast<SizeType>(s.length()), allocator);
        return m_value->PushBack(val.Move(), allocator);
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

    const std::vector<JsonValueRef> GetArrayValues() const
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

    const char* GetString(const char* str = nullptr) const
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

    SizeType GetStringLength() const { m_value->GetStringLength(); }

    template <typename Allocator>
    JsonValueRef SetString(std::string_view s, Allocator& allocator)
    {
        return m_value->SetString(s.data(), static_cast<SizeType>(s.length()), allocator);
    }

    template <typename T>
    bool Is() const { return m_value->Is<T>(); }

    template <typename T>
    T Get(const T& t = T()) const
    {
        if (IsValid())
        {
            return FromJson<T>(*this);
        }
        else
        {
            return t;
        }
    }

    template <typename T>
    std::vector<T> GetVector(const T& t = T()) const
    {
        if (IsArray())
        {
            std::vector<T> values;
            values.reserve((ArraySize()));
            for (auto jValue : GetArrayValues())
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
        return m_value->Set<T>(data, allocator);
    }

private:
    rapidjson::Value* m_value = nullptr;

}; // class JsonValueRef

template<>
bool FromJson(const JsonValueRef& json);
template<>
int32_t FromJson(const JsonValueRef& json);
template<>
uint32_t FromJson(const JsonValueRef& json);
template<>
int64_t FromJson(const JsonValueRef& json);
template<>
uint64_t FromJson(const JsonValueRef& json);
template<>
float FromJson(const JsonValueRef& json);
template<>
double FromJson(const JsonValueRef& json);
template<>
const char* FromJson(const JsonValueRef& json);
template<>
std::string FromJson(const JsonValueRef& json);

} // namespace rapid
