#include "scenes_loaders.h"
#include "network_server.h"

#include <random>

#include "game_engine/engine.h"
#include "game_engine/components/components.h"
#include "game_engine/systems/systems.h"

using namespace engn;

static float randf() {
    return (rand() % 1000) / 1000.0f;
}

void load_server_scene(engn::EngineContext& engine_ctx)
{
    auto& registry = engine_ctx.registry;

    registry.register_component<cpnt::Transform>();
    registry.register_component<cpnt::Velocity>();
    registry.register_component<cpnt::Player>();
    registry.register_component<cpnt::Enemy>();
    registry.register_component<cpnt::Bullet>();
    registry.register_component<cpnt::Health>();
    registry.register_component<cpnt::Hitbox>();
    registry.register_component<cpnt::MovementPattern>();
    registry.register_component<cpnt::Stats>();
    registry.register_component<cpnt::Replicated>();

    static std::unique_ptr<NetworkServer> s_server;
    if (!s_server) {
        constexpr uint16_t k_port = 8080;
        s_server = std::make_unique<NetworkServer>(engine_ctx, k_port);
        s_server->start();
    }

    engine_ctx.add_system<>([srv = s_server.get()](engn::EngineContext&) {
        srv->poll();
    });
}
