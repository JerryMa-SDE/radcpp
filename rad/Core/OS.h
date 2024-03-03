#pragma once

#include "Global.h"
#include "File.h"
#include <cstdlib>
#include <filesystem>
#include <boost/process/env.hpp>

namespace rad
{
namespace os
{

using FilePath = std::filesystem::path;
using FileType = std::filesystem::file_type;
using FileStatus = std::filesystem::file_status;
using FileTime = std::filesystem::file_time_type;

using FileSpaceInfo = std::filesystem::space_info;
using FileCopyOptions = std::filesystem::copy_options;
using FilePermissions = std::filesystem::perms;
using FilePermissionOptions = std::filesystem::perm_options;

using DirectoryEntry = std::filesystem::directory_entry;
using DirectoryIterator = std::filesystem::directory_iterator;
using RecursiveDirectorIterator = std::filesystem::recursive_directory_iterator;
using DirectoryOptions = std::filesystem::directory_options;

using FileSystemError = std::filesystem::filesystem_error;

// Python os interfaces

// os.environ
extern boost::process::native_environment g_environ;
std::string getenv(std::string_view name);
bool putenv(std::string_view key, std::string_view value);

FilePath getcwd();
void chdir(const FilePath& p);
std::vector<std::string> get_exec_path();

uint32_t getpid();
FilePath GetCurrentProcessPath();
std::string GetCurrentProcessName();

std::vector<FilePath> listdrives();

// File System

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
bool IsEquivalent(const FilePath& p1, const FilePath& p2);

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

// File types
bool IsBlockFile(const FilePath& p);
bool IsCharacterFile(const FilePath& p);
bool IsDirectory(const FilePath& p);
bool IsEmpty(const FilePath& p);
bool IsFIFO(const FilePath& p);
bool IsOther(const FilePath& p);
bool IsRegularFile(const FilePath& p);
bool IsSocket(const FilePath& p);
bool IsSymlink(const FilePath& p);
bool IsStatusKnown(const FileStatus& s);

// The string length cannot exceeds 1024.
std::string GetPathTimeString(const FilePath& path, std::string_view format = "%F %T");

} // namespace os
} // namespace rad
