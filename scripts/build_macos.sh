#!/usr/bin/env bash
# Standalone macOS build script - configures and builds the project using CMake

set -e

# Color codes for terminal output
RED="\033[1;31m"
GREEN="\033[1;32m"
YELLOW="\033[1;33m"
BLUE="\033[1;34m"
RESET="\033[0m"

# Start of the build script
echo -e "${BLUE}Starting macOS build script${RESET}"

# Create build directory if it doesn't exist
echo -e "${BLUE}Setting up build directory...${RESET}"
mkdir -p build
cd build
echo -e "${GREEN}Build directory ready.${RESET}"

# Prompt user for build type
read -p "$(echo -e "${YELLOW}Build type (Release/Debug) [Release]: ${RESET}")" build_type
build_type=${build_type:-Release}

# Validate build type
if [ "$build_type" != "Release" ] && [ "$build_type" != "Debug" ]; then
    echo -e "${RED}Invalid build type. Choose 'Release' or 'Debug'.${RESET}"
    exit 1
fi

# Run CMake to configure and build the project
echo -e "${BLUE}Configuring project...${RESET}"
cmake .. -DCMAKE_BUILD_TYPE=$build_type
echo -e "${BLUE}Building project...${RESET}"
cmake --build . -j
echo -e "${GREEN}Build succeeded${RESET}"

# Copy binaries to parent directory
echo -e "${BLUE}Copying executables...${RESET}"
cp bin/$build_type/r-type_client ../r-type_client
cp bin/$build_type/r-type_server ../r-type_server
echo -e "${GREEN}Executables copied to parent directory.${RESET}"

# Ask user if they want to run the program
echo -e "${YELLOW}Do you want to run the program now ? (y/N)${RESET}"
read answer

# Function to launch command in a terminal emulator
launch_in_terminal() {
    local cmd="$1"
    local title="$2"
    
    # Check if iTerm2 is available
    if [ -d "/Applications/iTerm.app" ]; then
        echo "Launching with iTerm2"
        osascript <<EOF
tell application "iTerm"
    activate
    set newWindow to (create window with default profile)
    tell current session of newWindow
        write text "cd \"$(pwd)\" && $cmd"
    end tell
end tell
EOF
        return 0
    fi
    
    # Fall back to default Terminal.app
    echo "Launching with Terminal.app"
    osascript <<EOF
tell application "Terminal"
    activate
    do script "cd \"$(pwd)\" && $cmd"
end tell
EOF
    return 0
}

# Run the client and server in separate terminal windows if user agreed
if [[ "$answer" == "y" || "$answer" == "Y" ]]; then
    cd ..
    echo -e "${BLUE}Launching programs...${RESET}"
    launch_in_terminal "echo 'Running $build_type ./r-type_client'; ./r-type_client" "R-Type Client"
    sleep 1  # Small delay to ensure windows open in order
    launch_in_terminal "echo 'Running $build_type ./r-type_server -p 8080'; ./r-type_server -p 8080" "R-Type Server"
else
    echo -e "${BLUE}Build complete. You can run the programs manually from the parent directory.${RESET}"
fi
