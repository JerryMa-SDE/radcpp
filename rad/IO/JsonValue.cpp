#include "JsonValue.h"

namespace rad
{

template <>
bool FromJson(const JsonValueRef& json)
{
    if (json.IsBool())
    {
        return json.GetBool();
    }
    return false;
}

template <>
int32_t FromJson(const JsonValueRef& json)
{
    if (json.IsInt())
    {
        return json.GetInt();
    }
    return 0;
}

template <>
uint32_t FromJson(const JsonValueRef& json)
{
    if (json.IsUint())
    {
        return json.GetUint();
    }
    return 0;
}

template <>
int64_t FromJson(const JsonValueRef& json)
{
    if (json.IsInt64())
    {
        return json.GetInt64();
    }
    return 0;
}

template <>
uint64_t FromJson(const JsonValueRef& json)
{
    if (json.IsUint64())
    {
        return json.GetUint64();
    }
    return 0;
}

template <>
float FromJson(const JsonValueRef& json)
{
    if (json.IsFloat())
    {
        return json.GetFloat();
    }
    return 0.0f;
}

template <>
double FromJson(const JsonValueRef& json)
{
    if (json.IsDouble())
    {
        return json.GetDouble();
    }
    return 0.0;
}

template<>
const char* FromJson(const JsonValueRef& json)
{
    if (json.IsString())
    {
        return json.GetString();
    }
    return nullptr;
}

template<>
std::string FromJson(const JsonValueRef& json)
{
    if (json.IsString())
    {
        return json.GetString();
    }
    return {};
}

int JsonValueRef::GetInt(int i) const
{
    if (IsValid())
    {
        if (IsInt())
        {
            return m_value->GetInt();
        }
        else if (IsString())
        {
            std::string_view str(GetString(), GetStringLength());
            if (rad::StrIsBinNumber(str))
            {
                return static_cast<int64_t>(std::strtol(str.substr(2).data(), nullptr, 2));
            }
            return static_cast<int64_t>(std::strtol(str.data(), 0, 0));
        }
        else
        {
            return i;
        }
    }
    else
    {
        return i;
    }
}

uint32_t JsonValueRef::GetUint(uint32_t u) const
{
    if (IsValid())
    {
        if (IsUint())
        {
            return m_value->GetUint();
        }
        else if (IsInt())
        {
            return m_value->GetInt();
        }
        else if (IsString())
        {
            std::string_view str(GetString(), GetStringLength());
            if (rad::StrIsBinNumber(str))
            {
                return static_cast<uint32_t>(std::strtoul(str.substr(2).data(), nullptr, 2));
            }
            return static_cast<uint32_t>(std::strtoul(str.data(), 0, 0));
        }
        else
        {
            return u;
        }
    }
    else
    {
        return u;
    }
}

int64_t JsonValueRef::GetInt64(int64_t i64) const
{
    if (IsValid())
    {
        if (IsInt64())
        {
            return m_value->GetInt64();
        }
        else if (IsString())
        {
            std::string_view str(GetString(), GetStringLength());
            if (rad::StrIsBinNumber(str))
            {
                return static_cast<int64_t>(std::strtoll(str.substr(2).data(), nullptr, 2));
            }
            return static_cast<int64_t>(std::strtoll(str.data(), 0, 0));
        }
        else
        {
            return i64;
        }
    }
    else
    {
        return i64;
    }
}

uint64_t JsonValueRef::GetUint64(int64_t u64) const
{
    if (IsValid())
    {
        if (IsUint64())
        {
            return m_value->GetUint64();
        }
        else if (IsInt64())
        {
            return m_value->GetInt64();
        }
        else if (IsString())
        {
            std::string_view str(GetString(), GetStringLength());
            if (rad::StrIsBinNumber(str))
            {
                return static_cast<uint64_t>(std::strtoull(str.substr(2).data(), nullptr, 2));
            }
            return static_cast<uint64_t>(std::strtoull(str.data(), 0, 0));
        }
        else
        {
            return u64;
        }
    }
    else
    {
        return u64;
    }
}

} // namespace rad
