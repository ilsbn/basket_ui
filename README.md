# Basket UI

A C++20 / Qt 6 Widgets application with a basket overview prototype.

The overview shell is defined in `src/basket_overview.ui`. Each tile uses the same
`BasketWidget` and `src/basket_widget.ui` form, editable in Qt Designer. The overview
receives a `BasketArray` (`std::array<BasketDetails, 20>`) and creates one tile per
entry, in array order. Each tile takes a `BasketDetails` struct containing `index`,
`sequence`, and `BasketStatus status`. Sample data is supplied by the main window;
the widgets own copies of their data without retaining caller references.
Sequence and status edits emit `BasketWidget::basketChanged(BasketDetails)`.
The overview updates the corresponding array position and emits
`BasketOverview::basketsChanged(BasketArray)`. `MainWindow` keeps the latest array,
available through `baskets()`, and emits its own `basketsChanged` signal for
persistence code to connect to. Queued connections receive snapshots of each edit.
Changing status also updates the tile colour. The overview's **In progress** and
**Pending** fields accept non-negative integers, independent of tile statuses.
They emit `inProgressChanged(int)` and `pendingChanged(int)`; basket type emits
`basketTypeChanged(QString)` for standard, premium, or platinum.
`MainWindow::overviewSettings()` exposes all three current values, and
`overviewSettingsChanged(BasketOverviewSettings)` provides a complete snapshot
after each edit for persistence. Storage and loading are not implemented yet.

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

## Run from VS Code (Windows)

Open this project folder in VS Code and install the recommended **C/C++** and
**CMake Tools** extensions. Ensure `VCPKG_ROOT` is set in VS Code's environment
(restart VS Code after changing it).

In **Run and Debug**, select **Basket UI (Debug)** or **Basket UI (Release)**,
then press **F5** to debug or **Ctrl+F5** to run without debugging. Each launch
configures and builds the matching CMake preset before starting the application.
The Debug configuration supports source breakpoints; Release uses optimization.

The default **Ctrl+Shift+B** build task builds the Debug application.
These launch targets use the MSVC Windows debugger.

See the [VS Code C++ debugging reference](https://code.visualstudio.com/docs/cpp/launch-json-reference)
for launch configuration options.

## Layout

```text
CMakeLists.txt       Project settings and dependencies
CMakePresets.json    Debug/Release configure, build, and test presets
vcpkg.json           Pinned third-party dependency manifest
cmake/              Qt platform-plugin deployment helper
src/main.cpp        Command-line options, logging, and Qt event loop
src/main_window.*   Main window hosting the basket overview
src/basket_overview.* Qt Designer form and widget setup
src/basket_widget.* Reusable basket tile and its Qt Designer form
src/basket_details.h BasketDetails, BasketStatus, and the fixed-size BasketArray
tests/              GoogleTest/GoogleMock smoke tests and test entry point
```

Machine-specific overrides belong in the ignored `CMakeUserPresets.json`.

Build setup follows the [Qt CMake guide](https://doc.qt.io/qt-6/cmake-get-started.html)
and [vcpkg CMake integration](https://learn.microsoft.com/en-us/vcpkg/users/buildsystems/cmake-integration).
