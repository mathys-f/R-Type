#!/usr/bin/env bash
# Standalone Windows build script for MinGW/W64Devkit - configures and builds the project using CMake
# This script works with Git Bash or W64Devkit shell on Windows

set -e

# Color codes for terminal output
RED="\033[1;31m"
GREEN="\033[1;32m"
YELLOW="\033[1;33m"
BLUE="\033[1;34m"
RESET="\033[0m"

# Start of the build script
echo -e "${BLUE}Starting Windows build script (MinGW-w64)${RESET}"

# Check if MinGW-w64 is available
if ! command -v gcc &> /dev/null; then
    echo -e "${RED}Error: MinGW-w64 not found in PATH.${RESET}"
    echo -e "${RED}Please install W64Devkit or MinGW-w64 and add it to your PATH.${RESET}"
    echo -e "${YELLOW}Download W64Devkit from: https://github.com/skeeto/w64devkit/releases${RESET}"
    exit 1
fi

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Error: CMake not found in PATH.${RESET}"
    echo -e "${RED}Please install CMake and add it to your PATH.${RESET}"
    exit 1
fi

# Create build directory if it doesn't exist
echo -e "${BLUE}Setting up build directory...${RESET}"
mkdir -p build
cd build
echo -e "${GREEN}Build directory ready.${RESET}"

# Prompt user for build type
read -p "$(echo -e "${YELLOW}Build type (Release/Debug) [Debug]: ${RESET}")" build_type
build_type=${build_type:-Debug}

# Validate build type
if [ "$build_type" != "Release" ] && [ "$build_type" != "Debug" ]; then
    echo -e "${RED}Invalid build type. Choose 'Release' or 'Debug'.${RESET}"
    exit 1
fi

# Run CMake to configure and build the project
echo -e "${BLUE}Configuring project...${RESET}"
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=$build_type -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
echo -e "${BLUE}Building project...${RESET}"
cmake --build .
echo -e "${GREEN}Build succeeded${RESET}"

# Copy binaries to parent directory
echo -e "${BLUE}Copying executables...${RESET}"
if [ -f "bin/$build_type/r-type_client.exe" ]; then
    cp bin/$build_type/r-type_client.exe ../r-type_client.exe
fi
if [ -f "bin/$build_type/r-type_server.exe" ]; then
    cp bin/$build_type/r-type_server.exe ../r-type_server.exe
fi
echo -e "${GREEN}Executables copied to parent directory.${RESET}"

# Ask user if they want to run the program
echo -e "${YELLOW}Do you want to run the program now ? (y/N)${RESET}"
read answer

# Run the client and server in separate windows if user agreed
if [[ "$answer" == "y" || "$answer" == "Y" ]]; then
    cd ..
    echo -e "${BLUE}Launching programs...${RESET}"
    
    # Launch server in new cmd window
    cmd.exe /c start "R-Type Server" cmd /k "echo Running $build_type r-type_server && r-type_server.exe -p 8080"
    
    # Wait a moment for server to start
    sleep 2
    
    # Launch client in new cmd window
    cmd.exe /c start "R-Type Client" cmd /k "echo Running $build_type r-type_client && r-type_client.exe"
    
    echo -e "${GREEN}Programs launched in separate windows.${RESET}"
else
    cd ..
    echo -e "${BLUE}Build complete. You can run the programs manually:"
    echo -e "  - ./r-type_server.exe -p 8080"
    echo -e "  - ./r-type_client.exe${RESET}"
fi
