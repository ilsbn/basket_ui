# Basket UI

A minimal C++20 / Qt 6 Widgets application that opens an empty main window.

## Prerequisites

- CMake 3.25 or newer and Ninja.
- A C++20 compiler. On Windows, install Visual Studio's **Desktop development
  with C++** workload and use an **x64 Native Tools Command Prompt** or
  **Developer PowerShell** so the compiler, CMake, and Ninja are on `PATH`.
- vcpkg, with `VCPKG_ROOT` pointing to its installation directory.
- Network access for the initial dependency download.

For example, in PowerShell, if vcpkg is not already configured:

```powershell
$env:VCPKG_ROOT = 'C:/path/to/vcpkg'
```

The manifest pins a vcpkg baseline and installs Qt Widgets, Boost.Program_options,
spdlog, and GoogleTest (including GoogleMock). No separate Qt installation or
global `vcpkg integrate install` is needed. The first configure builds dependencies
from source and can take a while. Both presets share `build/vcpkg_installed`.

## Build and test

From the project root:

```powershell
cmake --preset debug
cmake --build --preset debug
ctest --preset debug
```

For an optimized build:

```powershell
cmake --preset release
cmake --build --preset release
ctest --preset release
```

CTest runs the GoogleTest window tests with `QT_QPA_PLATFORM=minimal`, so they
do not need a display. The tests also use GoogleMock matchers. Additional CTest
checks verify that `--help` exits successfully and invalid options fail.

To build without the test target, configure with
`cmake --preset release -DBUILD_TESTING=OFF`.

## Run

On Windows:

```powershell
./build/debug/basket_ui.exe
./build/debug/basket_ui.exe --help
```

The app keeps console output available for spdlog and command-line help.
Close the window to exit. vcpkg copies the linked runtime DLLs during the build;
the project's CMake helper copies the matching native and headless Qt platform
plugins into `platforms/` beside each Windows executable.

The presets also work with Ninja on Linux and macOS after installing the native
build prerequisites required by Qt/vcpkg. The executable on those platforms is
`./build/debug/src/basket_ui`. Native Linux GUI support enables Qt's XCB feature.

## Layout

```text
CMakeLists.txt       Project settings and dependencies
CMakePresets.json    Debug/Release configure, build, and test presets
vcpkg.json           Pinned third-party dependency manifest
cmake/              Qt platform-plugin deployment helper
src/main.cpp        Command-line options, logging, and Qt event loop
src/main_window.*   Empty QMainWindow subclass
tests/              GoogleTest/GoogleMock smoke tests and test entry point
```

Machine-specific overrides belong in the ignored `CMakeUserPresets.json`.

Build setup follows the [Qt CMake guide](https://doc.qt.io/qt-6/cmake-get-started.html)
and [vcpkg CMake integration](https://learn.microsoft.com/en-us/vcpkg/users/buildsystems/cmake-integration).
