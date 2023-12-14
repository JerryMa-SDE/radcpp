#pragma once

#include "rad/Core/Global.h"
#include "rad/Core/String.h"
#include <filesystem>

namespace rad {

using FileSystemError = std::filesystem::filesystem_error;

using FilePath = std::filesystem::path;

using DirectoryEntry = std::filesystem::directory_entry;
using DirectoryIterator = std::filesystem::directory_iterator;
using RecursiveDirectorIterator = std::filesystem::recursive_directory_iterator;
using DirectoryOptions = std::filesystem::directory_options;

using FileType = std::filesystem::file_type;
using FileStatus = std::filesystem::file_status;
using FileTime = std::filesystem::file_time_type;

using FileSpaceInfo = std::filesystem::space_info;
using FilePermissions = std::filesystem::perms;
using FilePermissionOptions = std::filesystem::perm_options;
using FileCopyOptions = std::filesystem::copy_options;

FilePath GetCurrentPath();
FilePath GetWorkingDirectory();
void SetCurrentPath(const FilePath& p);
FilePath GetTempDirectory();

FilePath MakeAbsolute(const FilePath& p);
FilePath MakeRelative(const FilePath& p, const FilePath& base);
FilePath MakeProximate(const FilePath& p, const FilePath& base);
FilePath MakeCanonical(const FilePath& p);
FilePath MakeWeaklyCanonical(const FilePath& p);

bool Exists(const FilePath& p);
bool IsPathEquivalent(const FilePath& p1, const FilePath& p2);

std::uintmax_t GetFileSize(const FilePath& p);
FileStatus GetFileStatus(const FilePath& p);
FileStatus GetSymlinkStatus(const FilePath& p);
bool Exists(FileStatus s);
std::uintmax_t GetHardLinkCount(const FilePath& p);
FileTime GetLastWriteTime(const FilePath& p);
void SetLastWriteTime(const FilePath& p, FileTime t);

void SetPermissions(const FilePath& p, FilePermissions permissions,
    FilePermissionOptions options = FilePermissionOptions::replace);

FilePath ReadSymlink(const FilePath& p);

bool CreateDirectory(const FilePath& p);
bool CreateDirectory(const FilePath& p, const FilePath& copyAttribs);
bool CreateDirectories(const FilePath& p);

void CreateHardLink(const FilePath& target, const FilePath& link);
void CreateSymlink(const FilePath& target, const FilePath& link);
void CreateDirectorySymlink(const FilePath& target, const FilePath& link);

void Copy(const FilePath& from, const FilePath& to, FileCopyOptions options = FileCopyOptions::none);
void CopyFile(const FilePath& from, const FilePath& to, FileCopyOptions options = FileCopyOptions::none);
void CopySymlink(const FilePath& from, const FilePath& to);

// Delete file or empty directory, the same as POSIX remove.
// Symlinks are not followed (symlink is removed, not its target).
// Return true if the file was deleted, false if it did not exist.
bool Remove(const FilePath& p);

// Delete file or directory with all its subdirectories, recursively.
// Symlinks are not followed (symlink is removed, not its target).
// Return the number of files and directories that were deleted.
std::uintmax_t RemoveAll(const FilePath& p);

void Rename(const FilePath& oldPath, const FilePath& newPath);

void ResizeFile(const FilePath& p, std::uintmax_t newSize);

// Return the space info, the same as POSIX statvfs.
FileSpaceInfo GetSpaceInfo(const FilePath& p);

std::vector<FilePath> GetRootDirectories();
FilePath GetCurrentProcessPath();

// The string length cannot exceeds 1024.
std::string GetPathTimeString(const FilePath& path, std::string_view format = "%F %T");

enum FileAccessBits : uint32_t
{
    FileAccessRead = 0x00000001,
    FileAccessWrite = 0x00000002,
    FileAccessAppend = 0x00000004,
    FileAccessBinary = 0x00000008,
    FileAccessReadWrite = FileAccessRead | FileAccessWrite,
};

using FileAccessFlags = uint32_t;

class File
{
public:
    File();
    ~File();

    const FilePath& GetPath() const { return m_path; }
    std::FILE* GetHandle() const { return m_handle; }

    bool Open(const FilePath& filePath, FileAccessFlags flags);
    void Close();
    bool IsOpen();
    void Flush();

    size_t Read(void* buffer, size_t sizeInBytes, size_t elementCount = 1);
    size_t ReadLine(void* buffer, size_t bufferSize);
    size_t ReadLine(std::string& buffer);
    size_t Write(const void* buffer, size_t sizeInBytes, size_t count = 1);

    int Print(const char* format, ...);

    int32_t GetChar();

    // Sets the file position indicator for the file stream stream to the value pointed to by offset.
    // @origin: any of RW_SEEK_SET, RW_SEEK_CUR, RW_SEEK_END;
    int64_t Seek(int64_t offset, int origin);
    int64_t Rseek(int64_t offset);
    void Rewind();
    void FastForward();

    int64_t GetSize();
    int64_t Tell();

    static std::string ReadAll(const FilePath& path);
    static std::vector<std::string> ReadLines(const FilePath& path);

private:
    FilePath m_path;
    std::FILE* m_handle = nullptr;

}; // class File

} // namespace rad
