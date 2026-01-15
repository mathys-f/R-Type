#include "lobby_manager.h"

#include "game_engine/engine.h"
#include "game_engine/components/components.h"
#include "game_engine/systems/systems.h"

using namespace engn;

void lobby_scene_loader(EngineContext &engine_ctx)
{
    auto &registry = engine_ctx.registry;

    // Sim
    registry.register_component<cpnt::Bullet>();
    registry.register_component<cpnt::Enemy>();
    registry.register_component<cpnt::Health>();
    registry.register_component<cpnt::Hitbox>();
    registry.register_component<cpnt::MovementPattern>();
    registry.register_component<cpnt::Player>();
    registry.register_component<cpnt::Stats>();
    registry.register_component<cpnt::Tag>();
    registry.register_component<cpnt::Transform>();
    // Net
    registry.register_component<cpnt::Replicated>();

    // Sim
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Bullet>(sys::bullet_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Bullet, cpnt::Enemy, cpnt::Health, cpnt::Player, cpnt::Hitbox>(
        sys::collision_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::MovementPattern, cpnt::Velocity>(sys::enemy_movement_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Enemy, cpnt::Health, cpnt::Sprite>(sys::enemy_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Player, cpnt::Sprite, cpnt::Velocity, cpnt::Health>(
        sys::player_control_system);
    // Net
    engine_ctx.add_system<cpnt::Replicated>(sys::create_snapshot_system);
    engine_ctx.add_system<>(sys::update_snapshots_system);
    engine_ctx.add_system<cpnt::Replicated>(sys::send_snapshot_to_client);
}
