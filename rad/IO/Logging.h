#pragma once

#include "rad/Core/Global.h"
#include "rad/IO/File.h"
#include <chrono>
#include <mutex>

namespace rad
{

#ifndef RAD_NO_LOGGING

enum class LogLevel
{
    Undefined,
    Verbose,
    Debug,
    Info,
    Warn,
    Error,
    Critical,
    Count
};

const char* GetLogLevelString(LogLevel level);

class LogCategory
{
public:
    LogCategory(std::string_view name, LogLevel outputLevel) :
        m_name(name),
        m_outputLevel(outputLevel)
    {
    }
    ~LogCategory()
    {
    }

    void SetOutputLevel(LogLevel outputLevel)
    {
        m_outputLevel = outputLevel;
    }

    void SetFlushLevel(LogLevel flushLevel)
    {
        m_flushLevel = flushLevel;
    }

    void Print(LogLevel level, const char* format, ...);
    void Write(const char* data, size_t sizeInBytes);
    void Flush();

protected:
    std::string m_name;
    LogLevel m_outputLevel;
    LogLevel m_flushLevel = LogLevel::Warn;

    static std::mutex s_outputMutex;
    static rad::File s_logFile;
    bool m_enableConsoleOutput = true;

}; // class LogCategory

extern LogCategory g_logGlobal;
#define LogGlobal(Level, Format, ...) rad::g_logGlobal.Print(rad::LogLevel::Level, Format, ##__VA_ARGS__)

#endif

} // namespace rad
