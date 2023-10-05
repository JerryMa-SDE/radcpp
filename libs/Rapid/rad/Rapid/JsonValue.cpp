#include "JsonValue.h"

namespace rapid
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

} // namespace rapid
