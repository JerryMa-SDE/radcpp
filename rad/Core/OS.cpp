#define _CRT_SECURE_NO_WARNINGS 1
#include "OS.h"
#include "Logging.h"
#include <cerrno>
#include <cstdarg>
#include <ctime>

#ifdef _WIN32
#include <Windows.h>
#ifdef CreateDirectory
#undef CreateDirectory
#endif
#ifdef CreateHardLink
#undef CreateHardLink
#endif
#ifdef CopyFile
#undef CopyFile
#endif
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

FilePath GetCurrentPath()
{
    return std::filesystem::current_path();
}

FilePath GetWorkingDirectory()
{
    return std::filesystem::current_path();
}

void SetCurrentPath(const FilePath& p)
{
    std::filesystem::current_path(p);
}

FilePath GetTempDirectory()
{
    return std::filesystem::temp_directory_path();
}

FilePath MakeAbsolute(const FilePath& p)
{
    return std::filesystem::absolute(p);
}

FilePath MakeRelative(const FilePath& p, const FilePath& base)
{
    return std::filesystem::relative(p, base);
}

FilePath MakeProximate(const FilePath& p, const FilePath& base)
{
    return std::filesystem::proximate(p, base);
}

FilePath MakeCanonical(const FilePath& p)
{
    return std::filesystem::canonical(p);
}

FilePath MakeWeaklyCanonical(const FilePath& p)
{
    return std::filesystem::weakly_canonical(p);
}

bool Exists(const FilePath& p)
{
    return std::filesystem::exists(p);
}

bool IsEquivalent(const FilePath& p1, const FilePath& p2)
{
    return std::filesystem::equivalent(p1, p2);
}

std::uintmax_t GetFileSize(const FilePath& p)
{
    return std::filesystem::file_size(p);
}

FileStatus GetFileStatus(const FilePath& p)
{
    return std::filesystem::status(p);
}

FileStatus GetSymlinkStatus(const FilePath& p)
{
    return std::filesystem::symlink_status(p);
}

bool Exists(FileStatus s)
{
    return std::filesystem::exists(s);
}

std::uintmax_t GetHardLinkCount(const FilePath& p)
{
    return std::filesystem::hard_link_count(p);
}

FileTime GetLastWriteTime(const FilePath& p)
{
    return std::filesystem::last_write_time(p);
}

void SetLastWriteTime(const FilePath& p, FileTime t)
{
    std::filesystem::last_write_time(p, t);
}

void SetPermissions(const FilePath& p, FilePermissions permissions, FilePermissionOptions options)
{
    std::filesystem::permissions(p, permissions, options);
}

FilePath ReadSymlink(const FilePath& p)
{
    return std::filesystem::read_symlink(p);
}

bool CreateDirectory(const FilePath& p)
{
    return std::filesystem::create_directory(p);
}

bool CreateDirectory(const FilePath& p, const FilePath& copyAttribs)
{
    return std::filesystem::create_directory(p, copyAttribs);
}

bool CreateDirectories(const FilePath& p)
{
    return std::filesystem::create_directories(p);
}

void CreateHardLink(const FilePath& target, const FilePath& link)
{
    return std::filesystem::create_hard_link(target, link);
}

void CreateSymlink(const FilePath& target, const FilePath& link)
{
    return std::filesystem::create_symlink(target, link);
}

void CreateDirectorySymlink(const FilePath& target, const FilePath& link)
{
    return std::filesystem::create_directory_symlink(target, link);
}

void Copy(const FilePath& from, const FilePath& to, FileCopyOptions options)
{
    std::filesystem::copy(from, to, options);
}

void CopyFile(const FilePath& from, const FilePath& to, FileCopyOptions options)
{
    std::filesystem::copy_file(from, to, options);
}

void CopySymlink(const FilePath& from, const FilePath& to)
{
    std::filesystem::copy_symlink(from, to);
}

// Delete file or empty directory, the same as POSIX remove.
// Symlinks are not followed (symlink is removed, not its target).
// Return true if the file was deleted, false if it did not exist.
bool Remove(const FilePath& p)
{
    return std::filesystem::remove(p);
}

// Delete file or directory with all its subdirectories, recursively.
// Symlinks are not followed (symlink is removed, not its target).
// Return the number of files and directories that were deleted.
std::uintmax_t RemoveAll(const FilePath& p)
{
    return std::filesystem::remove_all(p);
}

void Rename(const FilePath& oldPath, const FilePath& newPath)
{
    std::filesystem::rename(oldPath, newPath);
}

void ResizeFile(const FilePath& p, std::uintmax_t newSize)
{
    std::filesystem::resize_file(p, newSize);
}

// Return the space info, the same as POSIX statvfs.
FileSpaceInfo GetSpaceInfo(const FilePath& p)
{
    return std::filesystem::space(p);
}

bool IsBlockFile(const FilePath& p)
{
    return std::filesystem::is_block_file(p);
}

bool IsCharacterFile(const FilePath& p)
{
    return std::filesystem::is_character_file(p);
}

bool IsDirectory(const FilePath& p)
{
    return std::filesystem::is_directory(p);
}

bool IsEmpty(const FilePath& p)
{
    return std::filesystem::is_empty(p);
}

bool IsFIFO(const FilePath& p)
{
    return std::filesystem::is_fifo(p);
}

bool IsOther(const FilePath& p)
{
    return std::filesystem::is_other(p);
}

bool IsRegularFile(const FilePath& p)
{
    return std::filesystem::is_regular_file(p);
}

bool IsSocket(const FilePath& p)
{
    return std::filesystem::is_socket(p);
}

bool IsSymlink(const FilePath& p)
{
    return std::filesystem::is_symlink(p);
}

bool IsStatusKnown(const FileStatus& s)
{
    return std::filesystem::status_known(s);
}

std::string GetPathTimeString(const FilePath& path, std::string_view format)
{
    std::tm dateTime = {};

#ifdef _WIN32
    struct __stat64 fileStatus = {};
    _wstat64(path.c_str(), &fileStatus);
    localtime_s(&dateTime, &fileStatus.st_mtime);
#else
    struct stat fileStatus = {};
    stat(path.c_str(), &fileStatus);
    localtime_r(&fileStatus.st_mtime, &dateTime);
#endif
    std::string buffer(128, 0);
    size_t bytesWritten = strftime(buffer.data(), buffer.size(), format.data(), &dateTime);
    if (bytesWritten == 0)
    {
        buffer.resize(1024);
        bytesWritten = strftime(buffer.data(), buffer.size(), format.data(), &dateTime);
    }
    return buffer;
}

} // namespace os

} // namespace rad
