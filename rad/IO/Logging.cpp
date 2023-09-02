#include "rad/IO/Logging.h"
#include "rad/IO/File.h"

#include <cstdarg>

RAD_BEGIN_NAMESPACE

#ifndef RAD_NO_LOGGING

const char* g_logLevelStrings[int(LogLevel::Count)] =
{
    "Undefined",
    "Verbose",
    "Debug",
    "Info",
    "Warn",
    "Error",
    "Critical",
};

const char* GetLogLevelString(LogLevel level)
{
    return g_logLevelStrings[int(level)];
}

std::mutex LogCategory::s_outputMutex;
rad::File LogCategory::s_logFile;

void LogCategory::Print(LogLevel level, const char* format, ...)
{
    std::chrono::system_clock::time_point timestamp = std::chrono::system_clock::now();

    thread_local std::string message;
    va_list args;
    va_start(args, format);
    StrPrintInPlaceArgList(message, format, args);
    va_end(args);

    long long milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(
        timestamp.time_since_epoch()).count() % 1000;
    std::time_t timepoint = std::chrono::system_clock::to_time_t(timestamp);
    std::tm datetime = {};
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&datetime, &timepoint);
#else
    localtime_r(&timepoint, &datetime);
#endif

    thread_local std::string buffer;
    int charsPrinted = StrPrintInPlace(buffer, "[%02d:%02d:%02d.%03lld] %s: %s: %s\n",
        datetime.tm_hour, datetime.tm_min, datetime.tm_sec, milliseconds,
        m_name.data(), g_logLevelStrings[int(level)], message.data());

    if ((charsPrinted > 0) && (level >= m_outputLevel))
    {
        Write(buffer.data(), charsPrinted);
    }

    if (level >= m_flushLevel)
    {
        Flush();
    }
}

void LogCategory::Write(const char* data, size_t sizeInBytes)
{
    std::lock_guard lockGuard(s_outputMutex);

    if (!s_logFile.IsOpen())
    {
        std::string processName = (const char*)GetCurrentProcessPath()
            .filename().u8string().c_str();
        s_logFile.Open(processName + ".log", FileAccessWrite);
    }

    if (s_logFile.IsOpen())
    {
        s_logFile.Write(data, sizeInBytes);
    }

    if (m_enableConsoleOutput)
    {
        fprintf(stderr, "%s", data);
    }
}

void LogCategory::Flush()
{
    s_logFile.Flush();
}

#endif // #ifndef RAD_NO_LOGGING

RAD_END_NAMESPACE

RAD_LOG_CATEGORY_DEFINE(Global);
