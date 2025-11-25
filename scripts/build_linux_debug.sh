mkdir build -p

cd build

cmake .. -DCMAKE_BUILD_TYPE=Debug

cmake --build .

cp bin/Debug/R-Type ../R-Type-Debug
cp bin/Debug/R-Type-server ../R-Type-server-Debug

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

    launch_in_terminal "echo running Debug ./R-Type-Debug; ./R-Type-Debug"
    launch_in_terminal "echo running Debug ./R-Type-server-Debug -p 8080; ./R-Type-server-Debug -p 8080"
fi
