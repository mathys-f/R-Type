#include "scenes_loaders.h"

#include "game_engine/engine.h"
#include "game_engine/components/components.h"
#include "game_engine/systems/systems.h"
#include "systems/systems.h"

#include <random>

using namespace engn;

namespace {
constexpr int k_rand_range = 1000;
constexpr float k_rand_divisor = 1000.0f;
} // namespace

static float randf() {
    return static_cast<float>(rand() % k_rand_range) / k_rand_divisor;
}

void lobby_scene_loader(EngineContext &engine_ctx)
{
    auto &registry = engine_ctx.registry;

    // Sim
    registry.register_component<cpnt::Transform>();
    registry.register_component<cpnt::Velocity>();
    registry.register_component<cpnt::Health>();
    registry.register_component<cpnt::Hitbox>();
    registry.register_component<cpnt::Bullet>();
    registry.register_component<cpnt::BulletShooter>();
    registry.register_component<cpnt::Player>();
    registry.register_component<cpnt::Enemy>();
    registry.register_component<cpnt::MovementPattern>();
    registry.register_component<cpnt::Stats>();
    registry.register_component<cpnt::Tag>();
    registry.register_component<cpnt::EntityType>();
    // Net
    registry.register_component<cpnt::Replicated>();

    // Sim
    engine_ctx.add_system<cpnt::Transform, cpnt::Player, cpnt::Velocity>(sys::server_player_control_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::MovementPattern, cpnt::Velocity>(sys::server_enemy_movement_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Enemy, cpnt::Health>(sys::server_enemy_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Bullet>(sys::server_bullet_system);
    // Net
    engine_ctx.add_system<cpnt::Player>(sys::server_update_player_entities_system);
    engine_ctx.add_system<cpnt::Replicated>(sys::create_snapshot_system);
    engine_ctx.add_system<>(sys::update_snapshots_system);
    engine_ctx.add_system<cpnt::Replicated>(sys::send_snapshot_to_client_system);
    engine_ctx.add_system<>(sys::clear_tombstones_system);


    constexpr float k_dist_min = 0.1f;
    constexpr float k_dist_max = 0.8f;

    constexpr float k_enemy_sprite_x = 5.0f;
    constexpr float k_enemy_sprite_y = 6.0f;
    constexpr float k_enemy_sprite_width = 21.0f;
    constexpr float k_enemy_sprite_height = 23.0f;
    constexpr float k_enemy_scale = 5.0f;
    constexpr float k_enemy_hitbox_width = 15.0f;
    constexpr float k_enemy_hitbox_height = 18.0f;
    constexpr float k_pattern_base_speed = 201.0f;

    constexpr const int k_width = 1920;
    constexpr const int k_height = 1080;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(k_dist_min, k_dist_max);

    // Create enemies
    for (size_t i = 0; i < engine_ctx.k_max_charger; i++) {
        auto enemy = engine_ctx.registry.spawn_entity();

        float spawn_y = (float)GetRandomValue(engine_ctx.k_spawn_margin, k_height - engine_ctx.k_spawn_margin);
        float spawn_x = (float)GetRandomValue(k_width, k_width * 2);

        // Replicated
        engine_ctx.registry.add_component(enemy, cpnt::Replicated{static_cast<uint32_t>(enemy)});

        // Tag for archetype
        engine_ctx.registry.add_component(enemy, cpnt::EntityType{"charger"});

        // Position
        engine_ctx.registry.add_component(enemy, cpnt::Transform{spawn_x, spawn_y, 0, 0, 0, 0, 1, 1, 1});

        // Velocity
        engine_ctx.registry.add_component(
            enemy, cpnt::Velocity{-(engine_ctx.k_enemy_base_speed + randf() * engine_ctx.k_enemy_speed_variance), 0.0f, 0.0f, 0.0f, 0.0f});

        // Other components
        engine_ctx.registry.add_component(enemy, cpnt::Enemy{});
        engine_ctx.registry.add_component(enemy, cpnt::Health{engine_ctx.k_enemy_health, engine_ctx.k_enemy_health});

        // Create a **new MovementPattern instance** for this enemy
        cpnt::MovementPattern pat;
        pat.speed = k_pattern_base_speed + randf() * engine_ctx.k_pattern_speed_variance;
        pat.amplitude = (float)GetRandomValue(1, engine_ctx.k_pattern_amplitude_max);
        pat.frequency = dist(gen);
        pat.timer = 1.f;
        int pattern_nbr = GetRandomValue(0, 3);
        switch (pattern_nbr) {
            case 0:
                pat.type = cpnt::MovementPattern::PatternType::ZigZag;
                break;
            case 1:
                pat.type = cpnt::MovementPattern::PatternType::Straight;
                break;
            case 2:
                pat.type = cpnt::MovementPattern::PatternType::Sine;
                break;
            case 3:
                pat.type = cpnt::MovementPattern::PatternType::Dive;
                break;
        }
        pat.base_y = spawn_y;

        engine_ctx.registry.add_component(enemy, std::move(pat));
        engine_ctx.registry.add_component(enemy, cpnt::Hitbox{k_enemy_hitbox_width * k_enemy_scale, k_enemy_hitbox_height * k_enemy_scale,
                                                              k_enemy_sprite_width, k_enemy_sprite_height});
    }
}
