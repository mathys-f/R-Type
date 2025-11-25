mkdir build -p

cd build

cmake .. -DCMAKE_BUILD_TYPE=Release

cmake --build .

cp bin/R-Type ../
cp bin/R-Type-server ../

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

    launch_in_terminal "echo running Release ./R-Type; ./R-Type"
    launch_in_terminal "echo running Release ./R-Type-server -p 8080; ./R-Type-server -p 8080"
fi
