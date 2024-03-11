# radcpp
Great C++ Collections

## How to Build

### Prerequisites

- C++ compiler with C++20 support: [Compiler support for C++20](https://en.cppreference.com/w/cpp/compiler_support/20)
    - For Windows, recommend [Visual Studio 2022](https://visualstudio.microsoft.com/downloads/).
    - For Linux, recommend `g++-13`.
- [CMake](https://cmake.org/)
- [vcpkg](https://vcpkg.io/)
- [Python3](https://www.python.org/)

Install the following vcpkg packages, with proper triplet (recommend "x64-windows" for Windows):

- boost (1.80.0 and above)
- gtest
- spdlog
- imath
- glm
- opencl
- cpu-features
- sdl2[vulkan] (for libs/DirectMedia)

Remember to set environment variable `VCPKG_ROOT` to the root of vcpkg repo for convenience. 

### Generate Project Files with CMake

Windows Command Prompt (cmd):

```bash
cmake -S . -B build-win -DCMAKE_TOOLCHAIN_FILE="%VCPKG_ROOT%/scripts/buildsystems/vcpkg.cmake"
```

Windows PowerShell:

```bash
cmake -S . -B build-win -DCMAKE_TOOLCHAIN_FILE="$env:VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake"
```

Linux (Ubuntu):

```bash
# install gcc-13 and g++-13 if your compiler doesn't suppor C++20 features
sudo add-apt-repository -y ppa:ubuntu-toolchain-r/test
sudo apt install -y g++-13
# generate project files
cmake -S . -B build-lnx -DCMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" -DCMAKE_C_COMPILER=/usr/bin/gcc-13 -DCMAKE_CXX_COMPILER=/usr/bin/g++-13
# build
cd build-lnx
make
```

## Useful Resources

### Language

[Standard C++ (isocpp.org)](https://isocpp.org/)

[C++ Core Guidelines (isocpp.github.io)](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#main)

[Draft C++ Standard: Contents (eel.is)](https://eel.is/c++draft/)

[Sutter’s Mill – Herb Sutter on software development](https://herbsutter.com/)

### Libraries

[fffaraz/awesome-cpp: A curated list of awesome C++ (or C) frameworks, libraries, resources](https://github.com/fffaraz/awesome-cpp)
