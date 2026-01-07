# R-Type

## What is it ???

A multiplayer version of the classic R-Type game made by yours trully, Serveur Robuste's team.

## Build & Run

### Linux

#### Dependencies

Make sur you have these packages installed:

```
build-essentials \
cmake \
git \
libx11-dev \
libxrandr-dev \
libxinerama-dev \
libxcursor-dev \
libxi-dev \
libgl1-mesa-dev \
libasound2-dev \
mesa-common-dev
```

#### Build

Then you can just use the provided script to build & run both the client & server:

```bash
./scripts/build_linux.sh
```

Once this script has been ran, you should see both the client & server executable at the root of your project.

If not, you will find them under /build/bin/Debug/r-type_xxx or /build/bin/Release/r-type_xxx depending on what you choose while running the build script.


#### Run

```bash
# Launches the client
r-type_client
```

```bash
# launches the server
r-type_server -p 8080
```


### Windows

#### Prerequisites

1. **MinGW-w64 (W64Devkit)** - Download from [W64Devkit releases](https://github.com/skeeto/w64devkit/releases)
   - Extract W64Devkit to a directory (e.g., `C:\w64devkit`)
   - Add the `bin` folder to your PATH (e.g., `C:\w64devkit\bin`)

2. **CMake** - Download from [cmake.org](https://cmake.org/download/)
   - Make sure CMake is added to your PATH during installation

3. **Git** (optional but recommended) - For cloning the repository

#### Verify Installation

Open a terminal (Command Prompt, PowerShell, or W64Devkit shell) and run:

```bash
gcc --version
cmake --version
```

Both commands should display version information if installed correctly.

#### Build

You have two options to build the project:

**Option 1: Using the batch script (Command Prompt/PowerShell)**

```cmd
scripts\build_windows.bat
```

**Option 2: Using the bash script (Git Bash/W64Devkit shell)**

```bash
./scripts/build_windows.sh
```

The script will:
- Check for MinGW-w64 and CMake
- Configure the project with CMake
- Ask you to choose between Debug or Release build
- Build the project
- Copy executables to the root directory
- Optionally launch the client and server

Once the script has run, you should find both executables at the root of your project:
- `r-type_client.exe`
- `r-type_server.exe`

If not, they will be in `build\bin\Debug\` or `build\bin\Release\` depending on your build type.

#### Run

```cmd
# Launch the server
r-type_server.exe -p 8080
```

```cmd
# Launch the client (in another terminal)
r-type_client.exe
```

#### Troubleshooting

- **"gcc not found"**: Make sure MinGW-w64's bin directory is in your PATH
- **"cmake not found"**: Make sure CMake is installed and in your PATH
- **Build errors**: Try cleaning the build directory and rebuilding:
  ```cmd
  rmdir /s /q build
  scripts\build_windows.bat
  ```
