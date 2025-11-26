---
title: Overview
id: overview
slug: /setup/
---

# Project Build Overview

This section provides a high-level overview of the build process for the project.
Its goal is to help new developers understand what tools are used, how the project is organized, and what to expect before diving into platform-specific build instructions.

## What You Will Learn

By following the setup documentation, you will learn:
- The required tools and system dependencies
- How the repository is organized
- How CMake and CPM.cmake handle builds and third-party libraries
- How to build the client and server executables
- How to build on Linux and Windows
- Optional tools such as Clang-Format and Clang-Tidy
- How to ensure reproducible, consistent builds

## Build Philosophy

The project follows a few principles:

1. Out-of-source builds

All build artifacts are generated inside a dedicated `build/` directory.

2. Modern CMake

The build system uses:
- CMake ≥ 3.20
- C++20 standard
- Separate targets for client, server, ECS, and networking

3. Lightweight dependency management (CPM)

External libraries (e.g., raylib) are automatically fetched during configuration using [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake).

This means:
- No external installers
- No global dependencies
- Fully reproducible builds

4. Cross-platform goal

Linux is fully supported.
Windows support is planned and partially implemented in the build system.

## What Gets Built?

Two executables:

Client

Located at:
```bash
build/bin/<CONFIG>/client
```

Server

Located at:
```bash
build/bin/<CONFIG>/server
```

Supporting modules:
- ECS library
- Networking library

## Next Steps

[Prerequisites](/setup/prerequisites) to see what tools are needed to build the project.
