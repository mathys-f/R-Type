#!/usr/bin/env bash
# Stop on any error
set -e

# Color codes for terminal output
RED="\033[1;31m"
GREEN="\033[1;32m"
YELLOW="\033[1;33m"
BLUE="\033[1;34m"
RESET="\033[0m"

# Start of the build script
echo -e "${BLUE}Starting Linux build script${RESET}"

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

# Clean src directory for Debug builds
if [ "$build_type" = "Debug" ]; then
    echo -e "${BLUE}Cleaning build/src for fresh clang-tidy analysis...${RESET}"
    rm -rf ./src
    echo -e "${GREEN}Src directory cleaned.${RESET}"
fi

# Run CMake to configure and build the project
echo -e "${BLUE}Configuring project...${RESET}"
cmake .. -DCMAKE_BUILD_TYPE=$build_type
echo -e "${BLUE}Building project...${RESET}"
cmake --build .
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

    TERMINALS=(
        "x-terminal-emulator -e"
        "kitty -e"
        "gnome-terminal --"
        "konsole --noclose -e"
        "xfce4-terminal --hold -e"
        "xterm -hold -e"
        "lxterminal -e"
        "tilix -e"
        "mate-terminal -e"
        "alacritty -e"
        "ptyxis -e"
    )

    for term in "${TERMINALS[@]}"; do
        # Extract the binary name
        bin="${term%% *}"
        if command -v "$bin" &>/dev/null; then
            echo "Launching with: $term"
            $term bash -c "$cmd; echo 'Press enter to close...'; read" &
            return 0
        fi
    done

    echo -e "${RED}No supported terminal emulator found. Run the program manually.${RESET}"
    return 1
}

# Run the client and server in separate terminal windows if user agreed
if [[ "$answer" == "y" || "$answer" == "Y" ]]; then
    cd ..
        echo -e "${BLUE}Launching programs...${RESET}"
    launch_in_terminal "echo running $build_type ./r-type_client; ./r-type_client"
    launch_in_terminal "echo running $build_type ./r-type_server -p 8080; ./r-type_server -p 8080"
else
    echo -e "${BLUE}Build complete. You can run the programs manually from the parent directory.${RESET}"
fi
