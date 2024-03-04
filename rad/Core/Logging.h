#pragma once

#include "Global.h"
#include "String.h"
#include <ctime>
#include <chrono>

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

// If not satisfied, try spdlog: https://github.com/gabime/spdlog
class Logger
{
public:
    Logger(std::string_view name);
    ~Logger();

    // @param overwrite: if true, overwrite existing contents.
    static bool SetOutputFile(std::string_view fileName, bool overwrite = false);

    void SetOutputLevel(LogLevel outputLevel) { m_outputLevel = outputLevel; }
    void SetFlushLevel(LogLevel flushLevel) { m_outputLevel = flushLevel; }

    void EnableOutputToConsole(bool enable = true) { m_enableOutputToConsole = enable; }
    // Call SetOutputFile first.
    void EnableOutputToFile(bool enable = true) { m_enableOutputToFile = enable; }

    // Standard format specification: https://en.cppreference.com/w/cpp/utility/format/spec
    template<typename... Args>
    void Log(LogLevel level, std::string_view format, Args&&... args)
    {
#ifndef RAD_NO_LOGGING
        std::chrono::system_clock::time_point tp = std::chrono::system_clock::now();
        long long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
            tp.time_since_epoch()).count() % 1000;
        std::time_t t = std::chrono::system_clock::to_time_t(tp);
        std::tm datetime = {};
#ifdef _WIN32
        localtime_s(&datetime, &t);
#else
        localtime_r(&t, &datetime);
#endif
        std::string message = std::vformat(
            std::string("[{:0>2}:{:0>2}:{:0>2}.{:0>3}] {}: {}: ").append(format).append("\n"),
            std::make_format_args(datetime.tm_hour, datetime.tm_min, datetime.tm_sec, milliseconds,
                m_name, GetLogLevelString(level), args...)
        );
        Output(level, message);
#endif // RAD_NO_LOGGING
    }

private:
    void Output(LogLevel level, std::string_view buffer);

    std::string m_name;
#ifdef _DEBUG
    LogLevel m_outputLevel = LogLevel::Debug;
#else
    LogLevel m_outputLevel = LogLevel::Info;
#endif
    LogLevel m_flushLevel = LogLevel::Warn;

    bool m_enableOutputToConsole = true;
    bool m_enableOutputToFile = false;

}; // Logger

extern Logger g_logger;

} // namespace rad

#ifndef RAD_NO_LOGGING
#define LogGlobal(Level, Format, ...) rad::g_logger.Log(rad::LogLevel::Level, Format, ##__VA_ARGS__)
#else
#define LogGlobal(Level, Format, ...)
#endif
