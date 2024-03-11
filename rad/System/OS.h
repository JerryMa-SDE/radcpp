#pragma once

#include "rad/Core/Global.h"
#include "FileSystem.h"
#include <boost/process/env.hpp>

namespace rad
{
namespace os
{

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

} // namespace os
} // namespace rad
