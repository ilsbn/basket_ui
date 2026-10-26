# Qt Widgets scaffold

Create a C++20 desktop application named Basket UI using Qt 6.8 or newer.
The application opens one empty `QMainWindow`, with no menus, toolbars, or content.

CMake 3.25 or newer owns the build. Ninja Debug and Release presets use
`VCPKG_ROOT` and a pinned vcpkg manifest for Qt Widgets, Boost.Program_options,
spdlog, and GoogleTest/GoogleMock. Keep window code in a small library so the
application and tests use the same implementation. Boost handles `--help`;
invalid options return a failure status, and spdlog logs application startup.

CTest discovers GoogleTest tests; GUI smoke tests use Qt's minimal headless platform.
GoogleMock matchers are available without adding artificial production interfaces.
Verify configure/build/test in both configurations and smoke-test startup.
The README documents prerequisites, commands, and the source layout.
