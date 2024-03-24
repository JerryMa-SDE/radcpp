#include "Logging.h"
#include "rad/Core/TypeTraits.h"
#include <cstdarg>
#include <fstream>
#include <mutex>

namespace rad
{

static const char* g_logLevelStrings[ToUnderlying(LogLevel::Count)] =
{
    "Debug",
    "Info",
    "Warn",
    "Error",
    "Critical",
};

const char* GetLogLevelString(LogLevel level)
{
    return g_logLevelStrings[ToUnderlying(level)];
}

static std::mutex g_logMutex;
static std::ofstream g_logFile;

bool SetLogFile(std::string_view fileName, bool overwrite)
{
    std::lock_guard lockGuard(g_logMutex);
    if (g_logFile.is_open())
    {
        g_logFile.close();
    }
    std::ios_base::openmode mode = std::ios_base::app;
    if (overwrite)
    {
        mode = std::ios_base::out;
    }
    g_logFile.open(fileName.data(), mode);
    return g_logFile.is_open();
}

Logger::Logger(std::string_view name)
{
    m_name = name;
}

Logger::~Logger()
{
}

void Logger::Output(LogLevel level, std::string_view buffer)
{
    std::lock_guard lockGuard(g_logMutex);

    if (m_enableOutputToConsole)
    {
        if (level <= LogLevel::Info)
        {
            fwrite(buffer.data(), buffer.size(), 1, stdout);
        }
        else
        {
            fwrite(buffer.data(), buffer.size(), 1, stderr);
        }
    }

    if (m_enableOutputToFile && g_logFile.is_open())
    {
        g_logFile.write(buffer.data(), buffer.size());
    }

    if (level >= m_flushLevel)
    {
        Flush();
    }
}

void Logger::Flush()
{
    fflush(stdout);
    g_logFile.flush();
}

Logger g_logGlobal = Logger("Global");

} // namespace rad
