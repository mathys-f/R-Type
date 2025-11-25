mkdir build -p

cd build

read -p "Build type (Release/Debug) [Debug]: " build_type
build_type=${build_type:-Debug}

if ([ "$build_type" != "Release" ] && [ "$build_type" != "Debug" ]); then
    echo "Invalid build type. Please choose 'Release' or 'Debug'."
    exit 1
fi

cmake .. -DCMAKE_BUILD_TYPE=$build_type

cmake --build .

cp bin/$build_type/r-type_client ../r-type_client
cp bin/$build_type/r-type_server ../r-type_server

echo "Do you want to run the program now ? (y/N)"

read answer

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

    echo "No supported terminal emulator found. Please run the program manually."
    return 1
}

if [[ "$answer" == "y" || "$answer" == "Y" ]]; then
    cd ..

    launch_in_terminal "echo running $build_type ./r-type_client; ./r-type"
    launch_in_terminal "echo running $build_type ./r-type_server -p 8080; ./r-type_server -p 8080"
fi
