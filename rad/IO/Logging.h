#pragma once

#include "rad/Core/Global.h"
#include "rad/IO/File.h"
#include <chrono>
#include <mutex>

namespace rad {

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
    LogCategory() = default;
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

#define RAD_LOG_CATEGORY(Name) g_logCategory##Name
#define RAD_LOG_CATEGORY_DECLARE(Name, OutputLevel) \
class LogCategory##Name : public rad::LogCategory \
{ \
public: \
    LogCategory##Name() : LogCategory(#Name, rad::LogLevel::OutputLevel) {} \
}; \
extern LogCategory##Name RAD_LOG_CATEGORY(Name);
#define RAD_LOG_CATEGORY_DEFINE(Name) LogCategory##Name RAD_LOG_CATEGORY(Name);
#define RAD_LOG(Name, Level, Format, ...) RAD_LOG_CATEGORY(Name).Print(rad::LogLevel::Level, Format, ##__VA_ARGS__)
#define RAD_LOG_SET_OUTPUT_LEVEL(Name, Level) RAD_LOG_CATEGORY(Name).SetOutputLevel(rad::LogLevel::Level)
#define RAD_LOG_SET_FLUSH_LEVEL(Name, Level) RAD_LOG_CATEGORY(Name).SetFlushLevel(rad::LogLevel::Level)

#else

#define RAD_LOG_CATEGORY(Name)
#define RAD_LOG_CATEGORY_DECLARE(Name, OutputLevel)
#define RAD_LOG_CATEGORY_DEFINE(Name)
#define RAD_LOG(CategoryName, Level, Format, ...)
#define RAD_SET_LOG_OUTPUT_LEVEL(Name, Level)
#define RAD_SET_LOG_FLUSH_LEVEL(Name, Level)

#endif

} // namespace rad

RAD_LOG_CATEGORY_DECLARE(Global, Verbose);
#ifndef LogGlobal
#define LogGlobal(Level, Format, ...) RAD_LOG(Global, Level, Format, ##__VA_ARGS__)
#endif
