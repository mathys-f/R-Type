# R-Type

> A robust multiplayer implementation of the classic R-Type game, powered by a custom Entity Component System (ECS) engine.

![Gameplay Screenshot](docs/static/img/gameplay.png)

*Gameplay screenshot.*

## 🚀 About The Project

R-Type is a side-scrolling shooter game where players control a spaceship to defeat enemies and obstacles. This project is a multiplayer implementation built from scratch using C++17/20.

It features a custom game engine based on the **ECS (Entity Component System)** architecture, ensuring high performance and modularity. The game uses a **Client-Server** architecture to handle multiplayer interactions, with UDP for real-time game state synchronization.

## ✨ Features

*   **Multiplayer**: Play with up to 4 friends in real-time.
*   **Custom ECS Engine**: High-performance architecture separating data (Components) from logic (Systems).
*   **Cross-Platform**: Runs on Linux, Windows, and macOS.
*   **Networking**: Robust UDP-based networking protocol with lag compensation.
*   **Audio**: Immersive sound effects and music.
*   **Customizable Controls**: Remap your keyboard or gamepad inputs.
*   **Documentation**: Comprehensive developer documentation.

## 🛠️ Build & Run

### Linux

#### Dependencies

Ensure you have the following packages installed:

```bash
build-essential
cmake
git
libx11-dev
libxrandr-dev
libxinerama-dev 
libxcursor-dev 
libxi-dev 
libgl1-mesa-dev 
libasound2-dev 
mesa-common-dev
```

#### Build & Run

Use the provided script to build and run both client and server:

```bash
./scripts/build_linux.sh
```

### macOS

#### Prerequisites

*   **Xcode Command Line Tools**: `xcode-select --install`
*   **CMake**: `brew install cmake`

#### Build & Run

Use the macOS build script:

```bash
./scripts/build_macos.sh
```

This script will compile the project and optionally launch the client and server for you.

### Windows

#### Prerequisites

1.  **MinGW-w64 (W64Devkit)** - Download from [W64Devkit releases](https://github.com/skeeto/w64devkit/releases)
2.  **CMake** - Download from [cmake.org](https://cmake.org/download/)

#### Build & Run

**Option 1: Batch script (Command Prompt/PowerShell)**
```cmd
scripts\build_windows.bat
```

**Option 2: Bash script (Git Bash/W64Devkit)**
```bash
./scripts/build_windows.sh
```

## 🎮 How to Play

### Controls

| Action | Keyboard (Primary) | Keyboard (Secondary) | Gamepad |
| :--- | :--- | :--- | :--- |
| **Move Up** | `W` | `Up Arrow` | D-Pad Up |
| **Move Down** | `S` | `Down Arrow` | D-Pad Down |
| **Move Left** | `A` | `Left Arrow` | D-Pad Left |
| **Move Right** | `D` | `Right Arrow` | D-Pad Right |
| **Shoot** | `Space` | - | Button South (A/X) |

*Controls can be customized in the Settings menu.*

### Launching the Game

1.  **Start the Server**:
    ```bash
    ./r-type_server -p 8080
    ```
2.  **Start the Client**:
    ```bash
    ./r-type_client
    ```
3.  **Connect**: In the client, enter the server IP (default localhost) and port (default 8080) to join the lobby.

## 📚 Documentation

The project documentation is built with Docusaurus.

To run the documentation locally:

1.  Navigate to the `docs` folder:
    ```bash
    cd docs
    ```
2.  Install dependencies:
    ```bash
    npm install
    ```
3.  Start the development server:
    ```bash
    npm start
    ```
    This will open the documentation in your browser at `http://localhost:3000`.

## 📂 Project Structure

*   `src/`: Source code for the game and engine.
    *   `client/`: Game client specific code (rendering, inputs).
    *   `server/`: Game server specific code (game loop, authoritative state).
    *   `ecs/`: Entity Component System implementation.
    *   `game_engine/`: Core engine systems and components.
    *   `networking/`: Network protocol implementation.
*   `assets/`: Game assets (sprites, music, sounds).
*   `docs/`: Project documentation.
*   `scripts/`: Build and utility scripts.
*   `web/`: Web dashboard (Frontend/Backend).

## 👥 Team

Developed by **Serveur Robuste's team**.