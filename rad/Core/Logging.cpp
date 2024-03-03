#include "Logging.h"
#include "OS.h"
#include <cstdarg>
#include <chrono>
#include <fstream>
#include <mutex>

namespace rad
{

static const char* g_logLevelStrings[int(LogLevel::Count)] =
{
    "Debug",
    "Info",
    "Warn",
    "Error",
    "Critical",
};

const char* GetLogLevelString(LogLevel level)
{
    return g_logLevelStrings[std::underlying_type_t<LogLevel>(level)];
}

std::ofstream g_logFile;
std::mutex g_logMutex;

void LogPrint(std::string_view category, LogLevel level, const char* format, ...)
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
        category.data(), g_logLevelStrings[int(level)], message.data());

    if (charsPrinted > 0)
    {
        std::lock_guard lockGuard(g_logMutex);

        if (!g_logFile.is_open())
        {
            std::string processName = os::GetCurrentProcessName();
            g_logFile.open(processName + ".log");
        }

        if (g_logFile.is_open())
        {
            g_logFile.write(buffer.data(), charsPrinted);
        }

        fprintf(stderr, "%s", buffer.data());
    }

    if (level >= LogLevel::Warn)
    {
        g_logFile.flush();
    }
}

} // namespace rad
