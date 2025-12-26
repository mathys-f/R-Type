#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cmath>
#include <optional>
#include <thread>
#include <chrono>
#include <cstring>

#include "game_engine/engine.h"
#include "game_engine/components/components.h"
#include "scenes_loaders.h"
#include "network_server.h"

using namespace engn;

int main(int argc, char** argv)
{
    // Parse port from args
    std::uint16_t port = 8080;
    for (int i = 1; i < argc; i++) {
        if (std::string(argv[i]) == "-p") {
            port = static_cast<std::uint16_t>(std::stoi(argv[i + 1]));
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
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    server.stop();
    return 0;
}
