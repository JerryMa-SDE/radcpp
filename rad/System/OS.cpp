#define _CRT_SECURE_NO_WARNINGS 1
#include "OS.h"
#include "rad/IO/Logging.h"

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace rad
{
namespace os
{

boost::process::native_environment g_environ = boost::this_process::environment();

std::string getenv(std::string_view name)
{
    std::string value;
#ifdef _WIN32
    char* pBuffer = nullptr;
    size_t bufferSize = 0;
    errno_t err = _dupenv_s(&pBuffer, &bufferSize, name.data());
    if (err == 0)
    {
        if (pBuffer)
        {
            value = pBuffer;
            free(pBuffer);
        }
    }
#else
    value = std::getenv(name.data());
#endif
    return value;
}

bool putenv(std::string_view key, std::string_view value)
{
#ifdef _WIN32
    errno_t err = _putenv_s(key.data(), value.data());
#else
    int err = ::setenv(key.data(), value.data(), 1);
#endif
    if (err == 0)
    {
        return true;
    }
    else
    {
        LogGlobal(Error, "Failed to set env %s: %s (%d)", key.data(), strerror(err), err);
        return false;
    }
}

FilePath getcwd()
{
    return std::filesystem::current_path();
}

void chdir(const FilePath& p)
{
    std::filesystem::current_path(p);
}

std::vector<std::string> get_exec_path()
{
    return g_environ.find("Path")->to_vector();
}

uint32_t getpid()
{
#ifdef _WIN32
    return GetCurrentProcessId();
#else
    return ::getpid();
#endif // _WIN32
}

FilePath GetCurrentProcessPath()
{
#ifdef _WIN32
    std::wstring buffer;
    buffer.resize(256);
    do {
        unsigned int len = GetModuleFileNameW(NULL, &buffer[0], static_cast<DWORD>(buffer.size()));
        if (len < buffer.size())
        {
            buffer.resize(len);
            break;
        }
        buffer.resize(buffer.size() * 2);
    } while (buffer.size() < 65536);
    return FilePath(buffer);
#else
    std::error_code ec;
    if (std::filesystem::exists("/proc/self/exe", ec))
    {
        return std::filesystem::read_symlink("/proc/self/exe", ec);
    }
    if (std::filesystem::exists("/proc/curproc/file", ec))
    {
        return std::filesystem::read_symlink("/proc/curproc/file", ec);
    }
    if (std::filesystem::exists("/proc/curproc/exe", ec))
    {
        return std::filesystem::read_symlink("/proc/curproc/exe", ec);
    }
    return std::to_string(getpid());
#endif
}

std::string GetCurrentProcessName()
{
    return (const char*)GetCurrentProcessPath().filename().u8string().c_str();
}

std::vector<FilePath> listdrives()
{
    std::vector<FilePath> drives;
#ifdef _WIN32
    DWORD mask = ::GetLogicalDrives();
    std::string drive = "A:\\";
    for (DWORD i = 0; i < sizeof(DWORD) * 8; i++)
    {
        if (mask & (1 << i))
        {
            drive[0] = 'A' + char(i);
            drives.push_back(drive);
        }
    }
#else
    drives.push_back(FilePath("/"));
#endif
    return drives;
}

bool SetConsoleOutputEncodingUTF8()
{
#ifdef _WIN32
    return ::SetConsoleOutputCP(65001);
#else
    return true;
#endif
}

} // namespace os
} // namespace rad
