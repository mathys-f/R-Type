#include "game_engine/components/components.h"
#include "game_engine/engine.h"
#include "lobby_manager.h"
#include "network_server.h"
#include "scenes_loaders.h"

#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <optional>
#include <thread>
#include <csignal>

using namespace engn;

bool g_running = true; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables)


void signal_handler(int signal) {
    if (signal == SIGINT) {
        g_running = false;
        LOG_INFO("Received SIGINT");
    }
}

void setup_signal_handling() {
    std::signal(SIGINT, signal_handler);
}

namespace {
constexpr std::uint16_t k_default_port = 8080;
constexpr int k_server_tick_ms = 16;              // ~60 updates per second
constexpr std::uint16_t k_lobby_base_port = 9000; // Lobbies will use ports starting from 9000
} // namespace

int main(int argc, char** argv) {
    // Parse command line arguments
    std::uint16_t port = k_default_port;
    bool is_lobby = false;
    std::uint32_t lobby_id = 0;
    
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-p") { // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            port = static_cast<std::uint16_t>(
                std::stoi(argv[i + 1])); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        } else if (std::string(argv[i]) == "-islobby") { // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            is_lobby = true;
        } else if (std::string(argv[i]) == "-lobby-id") { // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            lobby_id = static_cast<std::uint32_t>(
                std::stoul(argv[i + 1])); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }
    }

    if (is_lobby) {
        std::string lobby_name = "Lobby_" + std::to_string(lobby_id);
        GameLobby::run_lobby_in_child_process(lobby_id, lobby_name, port, 4);
        return 0;
    }

    setup_signal_handling();
    // Create engine context
    EngineContext engine_ctx;

    // Create lobby manager
    LobbyManager lobby_manager(k_lobby_base_port);

    // Start main network server (handles lobby management)
    NetworkServer server(engine_ctx, port, &lobby_manager);
    server.start();

    // Main server loop (headless)
    constexpr int k_cleanup_interval = 60; // Cleanup every 60 ticks (~1 second)
    int tick_count = 0;

    // Main server loop (headless)
    while (g_running) {
        server.poll();

        // Periodically cleanup empty lobbies
        if (++tick_count >= k_cleanup_interval) {
            lobby_manager.cleanup_empty_lobbies();
            tick_count = 0;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(k_server_tick_ms));
    }

    LOG_INFO("Shutting down server...");

    server.stop();
    return 0;
}
