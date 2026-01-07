#include "game_engine/components/components.h"
#include "game_engine/engine.h"
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
constexpr int k_server_tick_ms = 16; // ~60 updates per second
} // namespace

int main(int argc, char** argv) {
    // Parse port from args
    std::uint16_t port = k_default_port;
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-p") { // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            port = static_cast<std::uint16_t>(
                std::stoi(argv[i + 1])); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }
    }

    setup_signal_handling();
    // Create engine context
    EngineContext engine_ctx;

    // Don't use this scene yer, not ready
    // engine_ctx.add_scene_loader(0, load_server_scene);
    // engine_ctx.set_scene(0); // Server scene

    // Start network server
    NetworkServer server(engine_ctx, port);
    server.start();

    // Main server loop (headless)
    while (g_running) {
        server.poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(k_server_tick_ms));
    }

    LOG_INFO("Shutting down server...");

    server.stop();
    return 0;
}
