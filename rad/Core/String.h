#pragma once

#include "Global.h"

#include <cstring>
#include <string>
#include <string_view>
#include <vector>
#include <format>

#if defined(_MSC_BUILD) || defined(_MSC_VER)
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

namespace rad
{

// Use std::string by default (treat as UTF-8 encoded).
// https://utf8everywhere.org/
using String = std::string;

std::vector<std::string> StrSplit(std::string_view str, std::string_view delimiters, bool skipEmptySubStr = true);

std::string StrPrint(const char* format, ...);
int StrPrintInPlace(std::string& buffer, const char* format, ...);
int StrPrintInPlaceArgList(std::string& buffer, const char* format, va_list args);

template<typename... Args>
std::string StrFormat(std::string_view format, Args&&... args)
{
    return std::vformat(format, std::make_format_args(args...));
}

bool StrEqual(std::string_view str1, std::string_view str2);
bool StrCaseEqual(std::string_view str1, std::string_view str2);
int StrCompare(std::string_view left, std::string_view right);
int StrCaseCompare(std::string_view left, std::string_view right);

// case-insensitive string as key for std::set/map.
struct StringLessCaseInsensitive
{
    bool operator()(std::string_view left, std::string_view right) const
    {
        return (strcasecmp(left.data(), right.data()) < 0);
    }
};

std::string StrWideToU8(std::wstring_view wstr);
std::wstring StrU8ToWide(std::string_view str);

std::string StrUpper(std::string_view s);
std::string StrLower(std::string_view s);
void StrUpperInplace(std::string& s);
void StrLowerInplace(std::string& s);

bool StrIsDecInteger(std::string_view str);
bool StrIsHexNumber(std::string_view str);
bool StrIsBinNumber(std::string_view str);
// Check whether this string is a valid numeric string (a base 10 real number)
bool StrIsNumeric(std::string_view str);
bool IsDigit(char c);
bool IsHexDigit(char c);

std::string StrTrim(std::string_view str, std::string_view charlist = " \t\n\v\f\r");
void StrTrimInPlace(std::string& str, std::string_view charlist = " \t\n\v\f\r");

std::string StrReplace(std::string_view str, std::string_view subOld, std::string_view subNew);
void StrReplaceInPlace(std::string& str, std::string_view subOld, std::string_view subNew);

} // namespace rad
