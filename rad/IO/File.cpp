#include "File.h"
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

#else // Linux/Unix

#include <sys/stat.h>
#include <unistd.h>

#endif

namespace rad {

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

FilePath MakePathAbsolute(const FilePath& p)
{
    return std::filesystem::absolute(p);
}

FilePath MakePathRelative(const FilePath& p, const FilePath& base)
{
    return std::filesystem::relative(p, base);
}

FilePath MakePathProximate(const FilePath& p, const FilePath& base)
{
    return std::filesystem::proximate(p, base);
}

FilePath MakePathCanonical(const FilePath& p)
{
    return std::filesystem::canonical(p);
}

FilePath MakePathWeaklyCanonical(const FilePath& p)
{
    return std::filesystem::weakly_canonical(p);
}

bool Exists(const FilePath& p)
{
    return std::filesystem::exists(p);
}

bool IsPathEquivalent(const FilePath& p1, const FilePath& p2)
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

std::vector<FilePath> GetRootDirectories()
{
    std::vector<FilePath> rootDirectories;
#ifdef _WIN32
    DWORD logicalDrivesBitmask = ::GetLogicalDrives();
    std::string rootDirectory = "A:\\";
    for (DWORD i = 0; i < sizeof(DWORD) * 8; i++)
    {
        if (logicalDrivesBitmask & (1 << i))
        {
            rootDirectory[0] = 'A' + char(i);
            rootDirectories.push_back(rootDirectory);
        }
    }
#else
    rootDirectories.push_back(FilePath("/"));
#endif
    return rootDirectories;
}

FilePath GetCurrentProcessPath()
{
#ifdef _WIN32
    std::wstring buffer;
    buffer.resize(1024);
    do
    {
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

File::File()
{
}

File::~File()
{
    if (m_handle)
    {
        Close();
    }
}

bool File::Open(const FilePath& filePath, FileAccessFlags flags)
{
    const char* mode = "r";
    switch (flags)
    {
    case FileAccessRead:
        mode = "r";
        break;
    case FileAccessWrite:
        mode = "w";
        break;
    case FileAccessAppend:
        mode = "a";
        break;
    case (FileAccessRead | FileAccessAppend):
        mode = "a+";
        break;
    case (FileAccessRead | FileAccessWrite):
        mode = "w+";
        break;
    case (FileAccessRead | FileAccessBinary):
        mode = "rb";
        break;
    case (FileAccessWrite | FileAccessBinary):
        mode = "wb";
        break;
    case (FileAccessAppend | FileAccessBinary):
        mode = "ab";
        break;
    case (FileAccessRead | FileAccessAppend | FileAccessBinary):
        mode = "ab+";
        break;
    case (FileAccessRead | FileAccessWrite | FileAccessBinary):
        mode = "wb+";
        break;
    }

#if defined(_WIN32) || defined(_WIN64)
    errno_t err = fopen_s(&m_handle, (const char*)filePath.u8string().c_str(), mode);
#else
    m_handle = fopen((const char*)filePath.u8string().c_str(), mode);
    error_t err = 0;
    if (m_handle == nullptr)
    {
        err = errno;
    }
#endif
    if ((err == 0) && (m_handle != nullptr))
    {
        m_path = filePath;
        return true;
    }
    else
    {
        m_handle = nullptr;
        return false;
    }
}

void File::Close()
{
    fclose(m_handle);
    m_handle = nullptr;
}

bool File::IsOpen()
{
    return (m_handle != nullptr);
}

void File::Flush()
{
    fflush(m_handle);
}

size_t File::Read(void* buffer, size_t sizeInBytes, size_t elementCount)
{
    return fread(buffer, sizeInBytes, elementCount, m_handle);
}

size_t File::ReadLine(void* buffer, size_t bufferSize)
{
    size_t bytesRead = 0;
    char* str = static_cast<char*>(buffer);
    while (bytesRead < bufferSize)
    {
        int32_t c = GetChar();
        if ((c == '\n') || (c == EOF))
        {
            break;
        }
        str[bytesRead] = static_cast<char>(c);
        bytesRead++;
    }

    const size_t endIndex = ((bytesRead < bufferSize) ? bytesRead : (bufferSize - 1));
    str[endIndex] = '\0';

    return bytesRead;
}

size_t File::ReadLine(std::string& buffer)
{
    size_t bytesRead = 0;
    while (true)
    {
        int32_t c = GetChar();
        if ((c == '\n') || (c == EOF))
        {
            break;
        }
        buffer.push_back(static_cast<char>(c));
        bytesRead++;
    }
    return bytesRead;
}

size_t File::Write(const void* buffer, size_t sizeInBytes, size_t count)
{
    return fwrite(buffer, sizeInBytes, count, m_handle);
}

int File::Print(const char* format, ...)
{
    int ret = 0;
    va_list args;
    va_start(args, format);
    ret = vfprintf(m_handle, format, args);
    va_end(args);
    return ret;
}

int32_t File::GetChar()
{
    return std::getc(m_handle);
}

int64_t File::Seek(int64_t offset, int origin)
{
    return fseek(m_handle, static_cast<long>(offset), origin);
}

int64_t File::Rseek(int64_t offset)
{
    return fseek(m_handle, static_cast<long>(offset), SEEK_END);
}

void File::Rewind()
{
    std::rewind(m_handle);
}

void File::FastForward()
{
    Rseek(0);
}

int64_t File::GetSize()
{
    return static_cast<uint64_t>(GetFileSize(m_path));
}

int64_t File::Tell()
{
    return ftell(m_handle);
}

std::string File::ReadAll(const FilePath& path)
{
    File file;
    std::string buffer;
    if (file.Open(path, rad::FileAccessRead | rad::FileAccessBinary))
    {
        int64_t fileSize = file.GetSize();
        buffer.resize(fileSize);
        file.Read(buffer.data(), fileSize);
        return buffer;
    }
    return buffer;
}

std::vector<std::string> File::ReadLines(const FilePath& path)
{
    File file;
    std::vector<std::string> lines;
    if (file.Open(path, FileAccessRead))
    {
        std::string line;
        while (true)
        {
            int32_t c = file.GetChar();
            if ((c != '\n') && (c != EOF))
            {
                line.push_back(static_cast<char>(c));
            }
            else
            {
                lines.push_back(line);
                line.clear();
                if (c == EOF)
                {
                    break;
                }
            }
        }
    }
    return lines;
}

} // namespace rad
