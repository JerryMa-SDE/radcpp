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
- imath
- glm
- opencl

Call CMake to generate project files:
- Windows cmd:
```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake"
```
- Windows PowerShell
```bash
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
```
## Useful Resources

### Language

[Standard C++ (isocpp.org)](https://isocpp.org/)

[C++ Core Guidelines (isocpp.github.io)](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#main)

[Draft C++ Standard: Contents (eel.is)](https://eel.is/c++draft/)

[Sutter’s Mill – Herb Sutter on software development](https://herbsutter.com/)

### Libraries

[fffaraz/awesome-cpp: A curated list of awesome C++ (or C) frameworks, libraries, resources](https://github.com/fffaraz/awesome-cpp)
