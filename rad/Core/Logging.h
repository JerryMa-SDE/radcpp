#pragma once

#include "Global.h"
#include "String.h"

namespace rad
{

enum class LogLevel
{
    Debug,
    Info,
    Warn,
    Error,
    Critical,
    Count
};

const char* GetLogLevelString(LogLevel level);

// Print formatted message to stderr and ProcessName.log.
// If not satisfied, try spdlog: https://github.com/gabime/spdlog
void LogPrint(std::string_view category, LogLevel level, const char* format, ...);

} // namespace rad

#ifndef RAD_NO_LOGGING
#define LogGlobal(Level, Format, ...) rad::LogPrint("Global", rad::LogLevel::Level, Format, ##__VA_ARGS__)
#else
#define LogGlobal(Level, Format, ...)
#endif
