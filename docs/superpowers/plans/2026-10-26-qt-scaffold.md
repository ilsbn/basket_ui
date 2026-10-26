# Qt Widgets Scaffold Implementation Plan

> **For agentic workers:** Use superpowers:executing-plans to implement this plan inline.

**Goal:** Build an empty Qt main window with the requested dependency and testing tools.

**Architecture:** A widget library is shared by the executable and headless GUI tests.
Keep command-line parsing and startup logging in the executable.

**Tech Stack:** C++20, Qt 6, CMake, Ninja, vcpkg, Boost.Program_options, spdlog, GoogleTest/GoogleMock.

**Spec:** `docs/superpowers/specs/2026-10-26-qt-scaffold-design.md`

## Global Constraints

- CMake 3.25 or newer; Qt 6.8 or newer; C++20 without compiler extensions.
- Start with an empty main window; avoid unrelated features.
- Use an environment-based vcpkg toolchain and pinned dependency baseline.

## Tasks

- [x] Add the manifest, presets, CMake targets, and build-output ignore rules.
- [x] Add GUI smoke tests plus CTest checks for help and invalid options.
- [x] Configure the project and install the pinned dependencies.
- [x] Implement `basket::MainWindow` and application startup with Boost and spdlog.
- [x] Build Debug and Release, run CTest, and verify the native window opens and closes.
- [x] Document prerequisites, build/run/test commands, and review the scaffold.

## Verification

Verified on Windows x64 using MSVC 19.51 and Qt 6.11.0:

- `cmake --preset debug` and `cmake --build --preset debug`: passed.
- `ctest --preset debug`: 4/4 tests passed.
- `cmake --preset release` and `cmake --build --preset release`: passed.
- `ctest --preset release`: 4/4 tests passed.
- Native Windows startup and graceful window closure: passed in both configurations,
  from a separate working directory with no Qt plugin path overrides.
- Code review identified the Windows platform-plugin deployment requirement;
  the CMake helper copies the native and minimal headless plugins per configuration.
- Runtime verification identified duplicate GoogleTest DLL state when linking both
  GoogleTest and GoogleMock on MSVC. Linking `GTest::gmock` alone resolves discovery
  and provides both frameworks, as intended by their upstream CMake targets.
