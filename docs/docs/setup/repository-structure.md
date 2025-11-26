---
title: Repository Structure
id: repository-structure
---

# Repository Structure

This page describes how the project is organized, what each directory contains, and how the different components interact.
Understanding the layout makes it easier to navigate the codebase, find build targets, and contribute effectively.

## Top-Level Layout
```graphql
R-TYPE/
├── docs/                 # Documentation (Docusaurus)
├── scripts/              # Helper scripts for building and cleaning
├── src/                  # Main source code
├── CMakeLists.txt        # Main build configuration
└── ...
```

## `/src`

The source directory contains separate components and executables:

```graphql
src/
├── client/               # Client application
│   ├── CMakeLists.txt
│   └── main.cpp
│
├── server/               # Server application
│   ├── CMakeLists.txt
│   └── main.cpp
│
├── ecs/                  # ECS implementation exposed as a library
│   ├── CMakeLists.txt
│   ├── ecs.cpp
│   └── ecs.h
│
└── networking/           # Networking module (UDP/TCP, serialization, etc.)
    ├── CMakeLists.txt
    ├── networking.cpp
    └── networking.h
```

### Client
- Depends on: ECS, Networking, raylib
- Produces: r-type_client

### Server
- Depends on: ECS, Networking
- Produces: r-type_server

## `/scripts`

Helper scripts for Linux development and Windows later:

### `build_linux.sh`
- Creates the build directory
- Asks for build type (Debug/Release)
- Runs CMake configuration
- Builds all targets
- Copies executables (`r-type_client`, `r-type_server`) to project root
- Optionally launches client and server in terminal windows
- Detects and uses available terminal emulators (kitty, gnome-terminal, xterm, etc.)

### `clean_execs.sh`

Removes executables created by the build script

## `/docs`

Contains all Docusaurus documentation:
- Markdown pages
- Sidebar configuration
- Static assets
- Docusaurus config

## `CMakeLists.txt` (root)

The main CMake file handles:
- C++20 configuration
- Output directories
- Fetching CPM.cmake
- Fetching dependencies
- Adding subdirectories for ECS, Networking, Client, Server
- Formatting and static analysis integration (Clang-Format, Clang-Tidy)

This is the entry point for all builds.

## Summary

This repository follows a clean modular architecture:
- Client / Server = executables
- ECS / Networking = static libraries reused across modules
- CPM = automatic dependency manager
- Scripts = developer convenience tools
- Docs = complete documentation structure
