#pragma once

#include "rad/Core/Global.h"
#include <boost/json.hpp>
// Document Model
// array: sequence container of JSON values supporing dynamic size and fast, random access.
// object: associative container of key-value pairs with unique keys (string-value pairs).
// string: a contiguous range of characters (UTF-8).
// value: a special variant holds one of the six standard JSON data types.
// kind: array, object, string, int64, uint64, double, bool, null.

namespace rad
{

boost::json::value ParseJson(std::string_view str);
boost::json::value ParseJsonFromFile(std::string_view fileName);

// a helper class for json::value.
class JsonRef
{
public:
    JsonRef() : m_val(nullptr) {}
    JsonRef(const boost::json::value* val) : m_val(val) {}
    JsonRef(const boost::json::value& val) : m_val(&val) {}
    ~JsonRef() {}

    bool IsValid() const { return (m_val != nullptr); }
    operator bool() const { return IsValid(); }

    bool IsArray() const { return m_val->is_array(); }
    bool IsObject() const { return m_val->is_object(); }
    bool IsString() const { return m_val->is_string(); }
    bool IsInt() const { return m_val->is_int64(); }
    bool IsUint() const { return m_val->is_uint64(); }
    bool IsDouble() const { return m_val->is_double(); }
    bool IsBool() const { return m_val->is_bool(); }
    bool IsNull() const { return m_val->is_null(); }

    // Get with default.
    const char* GetString(const char* str = "") const;
    int64_t GetInt64(int64_t i = 0) const;
    int32_t GetInt32(int32_t i = 0) const;
    uint64_t GetUint64(uint64_t i = 0) const;
    uint32_t GetUint32(uint32_t i = 0) const;
    double GetDouble(double f = 0.0) const;
    float GetFloat(float f = 0.0f) const;
    double GetBool(bool b = false) const;

    JsonRef FindMember(std::string_view key);
    JsonRef FindMemberCaseInsensitive(std::string_view key);

private:
    const boost::json::value* m_val;

}; // class JsonRef

} // namespace rad
