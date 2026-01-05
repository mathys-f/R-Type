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

using namespace engn;

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

    // Create engine context
    EngineContext engine_ctx;

    // Start network server
    NetworkServer server(engine_ctx, port);
    server.start();

    // Main server loop (headless)
    while (true) {
        server.poll();
        std::this_thread::sleep_for(std::chrono::milliseconds(k_server_tick_ms));
    }

    server.stop();
    return 0;
}
