# radcpp
Great C++ Collections

## How to Build
Prerequisites:
- [CMake](https://cmake.org/)
- [Python3](https://www.python.org/)

Setup [Vcpkg](https://learn.microsoft.com/en-us/vcpkg/get_started/overview), set env VCPKG_ROOT to its path and install the following packages:
- boost
- gtest
- spdlog

Call CMake to generate project files:
- Windows cmd:
```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake"
```
- Windows PowerShell
```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
```
