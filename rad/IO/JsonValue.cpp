#include "JsonValue.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"

namespace rad
{

bool JsonValueRef::GetBool(bool b) const
{
    if (IsValid())
    {
        if (IsBool())
        {
            return m_value->GetBool();
        }
        else if (IsInt())
        {
            return m_value->GetInt();
        }
    }
    return b;
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
    }
    return i;
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
    }
    return u;
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
    }
    return i64;
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
    }
    return u64;
}

rapidjson::StringBuffer JsonValueRef::Stringify()
{
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    m_value->Accept(writer);
    return buffer;
}

rapidjson::StringBuffer JsonValueRef::StringifyPretty()
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    m_value->Accept(writer);
    return buffer;
}

void FromJson(const JsonValueRef& json, std::string& str)
{
    if (json.IsString())
    {
        str = json.GetString();
    }
}

} // namespace rad
