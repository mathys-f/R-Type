---
title: Technical Choices
id: technical-choices
---

# Technical Choices

This document explains the main technical choices made in the project: the package manager (`CPM`) and the graphics library (`raylib`). It summarizes the rationale, alternatives considered, and build/maintenance implications.

## Package Manager: CPM

- **Why CPM:** CPM is a single-file CMake script that integrates directly into CMake-based projects. It was chosen because it fits naturally with our CMake build system and requires minimal additional tooling for contributors.
- **Benefits:**
  - Simple integration: just include the `CPM.cmake` helper in the CMake flow.
  - Reproducible dependency fetching using CMake's FetchContent-style behavior.
  - No separate package manager or language-specific tooling to install.
  - Works well with our CI/build environments which already run CMake.
- **Implications:**
  - Dependencies are declared in CMake, keeping build configuration centralized.
  - Contributors only need a working CMake toolchain to fetch and build third-party libs.

## Alternatives Considered

- **Conan:** A mature, decentralized C/C++ package manager with wide community adoption. It offers binary package distribution, versioning, and fine-grained dependency control. It integrates with CMake via helper tools (e.g., `cmake-conan`) but requires contributors to install and manage an external tool, which adds setup overhead and CI changes compared to a single-file CMake approach.
- **vcpkg:** Microsoft's `vcpkg` provides a curated set of portable C/C++ libraries and integrates with CMake through a toolchain file. It simplifies cross-platform builds (especially on Windows) and can provide prebuilt binaries, but it introduces an additional dependency and workflow outside of CMake's native flow and can be less flexible for non-standard or cutting-edge dependency versions.

## Graphics Library: raylib

- **Why raylib:** raylib is designed specifically for game development, emphasizing simplicity and rapid iteration. It provides a lightweight, game-friendly API for graphics, input, audio, and textures, which makes it a good fit for an educational and prototype-friendly game project like this one.
- **Benefits:**
  - Easy to learn and use: concise C API that maps well to typical game loop code.
  - Lightweight and performant for 2D/3D game use-cases common to this project.
  - Good cross-platform support, which simplifies development on Linux/macOS/Windows.
  - Provides built-in helpers for textures, audio, input handling and basic 2D/3D rendering.
- **Implications:**
  - The repo already includes raylib sources and/or subbuild artifacts under the build tree, so CI and local builds will compile or reuse the integrated raylib as part of the normal CMake build.
  - Game-specific features (sprites, audio playback, input polling) are simpler to implement and maintain with raylib compared to more general-purpose GUI libraries.

## Alternatives Considered

- **SDL2 / SDL + SDL_image / SDL_mixer:** Very mature and flexible. More low-level; useful for broad multimedia needs. Chosen raylib instead because raylib offers a higher-level, game-focused API which speeds development.
- **SFML:** Also game-friendly and C++-oriented. We preferred raylib for its minimal C API, small footprint, and the ease of integrating simple game loops.

## Summary

We chose CPM for its seamless integration with CMake and low contributor overhead, and raylib for its game-focused, lightweight API and rapid development benefits. These choices prioritize fast iteration, simple contributor setup, and keeping build configuration centralized in CMake.
