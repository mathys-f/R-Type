---
title: Prerequisites
id: prerequisites
---

# Prerequisites

Before building the project, ensure that your system has the required tools installed.
This page lists all necessary software, optional developer tools, and platform-specific notes for Linux and Windows.

## Required Software

### CMake ≥ 3.20

Used as the main build system generator.

Check your version:
```bash
cmake --version
```

### C++20-Compatible Compiler

You need a compiler that supports C++20:

Linux
- GCC ≥ 11
- Clang ≥ 13

Recommended installation (Ubuntu/Debian):
```bash
sudo apt install build-essential
```

Windows

- MSVC (Visual Studio 2022 or newer)

Windows support is planned; MSVC is the recommended compiler once enabled.

### Git

Required to clone the repository and for CPM to download dependencies.

Check:
```bash
git --version
```

## Optional but Recommended Tools

These tools are automatically integrated into the CMake build system:

### Clang-Format

Used to automatically format the entire codebase.

Installation (Linux):
```bash
sudo apt install clang-format
```

Usage (after build configuration):
```bash
cmake --build build --target format
```

### Clang-Tidy

Static analysis and linting tool.
If available, CMake automatically enables it for all C++ targets.

Installation (Linux):
```bash
sudo apt install clang-tidy
```

## Platform Notes

### Linux

All build scripts are currently written for Linux and fully supported.

Ensure you have development packages for OpenGL / X11 when using raylib:
```bash
sudo apt install libx11-dev libxi-dev libxrandr-dev libxinerama-dev libxcursor-dev libgl1-mesa-dev
```
(If raylib fails to compile, these are usually the missing dependencies.)

### Windows

Windows builds are planned and partially configured in CMake.
You will eventually need:
- Visual Studio 2022
- “Desktop Development with C++” workload
- CMake integration (included in VS)

## Why These Tools?

The project relies on:
- Modern CMake for cross-platform build logic
- A modern C++ compiler for advanced language features
- CPM.cmake, which requires Git to download dependencies
- Optional Clang tools for a consistent code style

These tools ensure reproducibility and consistent developer experience across environments.

## Next Steps

[Repository Structure](/setup/repository-structure) to understand how the codebase is organized.
