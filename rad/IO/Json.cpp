#include "Json.h"
#include "rad/Core/String.h"
#include "File.h"

namespace rad
{

void SetDefaultParseOptions(boost::json::parse_options& options)
{
    options.allow_comments = true;
    options.allow_trailing_commas = true;
    options.allow_infinity_and_nan = true;
}

boost::json::value ParseJson(std::string_view str)
{
    boost::json::parse_options options = {};
    SetDefaultParseOptions(options);
    return boost::json::parse(str, {}, options);
}

boost::json::value ParseJsonFromFile(std::string_view fileName)
{
    boost::json::parse_options options = {};
    SetDefaultParseOptions(options);
    return boost::json::parse(File::ReadAll(fileName), {}, options);
}

const char* JsonRef::GetString(const char* str) const
{
    if (m_val->is_string())
    {
        return m_val->get_string().c_str();
    }
    else
    {
        return str;
    }
}

int64_t JsonRef::GetInt64(int64_t i) const
{
    if (m_val->is_int64())
    {
        return m_val->get_int64();
    }
    else
    {
        return i;
    }
}

int32_t JsonRef::GetInt32(int32_t i) const
{
    if (m_val->is_int64())
    {
        return static_cast<int32_t>(m_val->get_int64());
    }
    else
    {
        return i;
    }
}

uint64_t JsonRef::GetUint64(uint64_t i) const
{
    if (m_val->is_uint64())
    {
        return m_val->get_uint64();
    }
    else
    {
        return i;
    }
}

uint32_t JsonRef::GetUint32(uint32_t i) const
{
    if (m_val->is_uint64())
    {
        return static_cast<uint32_t>(m_val->get_uint64());
    }
    else
    {
        return i;
    }
}

double JsonRef::GetDouble(double f) const
{
    if (m_val->is_double())
    {
        return m_val->get_double();
    }
    else
    {
        return f;
    }
}

float JsonRef::GetFloat(float f) const
{
    if (m_val->is_double())
    {
        return static_cast<float>(m_val->get_double());
    }
    else
    {
        return f;
    }
}

double JsonRef::GetBool(bool b) const
{
    if (m_val->is_bool())
    {
        return m_val->get_bool();
    }
    else
    {
        return b;
    }
}

JsonRef JsonRef::FindMember(std::string_view key)
{
    if (IsValid() && m_val->is_object())
    {
        auto iter = m_val->get_object().find(key);
        if (iter != m_val->get_object().end())
        {
            return iter->value();
        }
    }
    return JsonRef();
}

JsonRef JsonRef::FindMemberCaseInsensitive(std::string_view key)
{
    if (IsValid() && m_val->is_object())
    {
        for (const auto& iter : m_val->get_object())
        {
            if (rad::StrCaseEqual(iter.key(), key))
            {
                return iter.value();
            }
        }
    }
    return JsonRef();
}

} // namespace rad
