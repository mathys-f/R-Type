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
    return static_cast<float>(rand() % k_rand_range) / k_rand_divisor; // NOLINT(clang-analyzer-security.insecureAPI.rand)
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
    registry.register_component<cpnt::Shooter>();
    registry.register_component<cpnt::MovementPattern>();
    registry.register_component<cpnt::Stats>();
    registry.register_component<cpnt::Tag>();
    registry.register_component<cpnt::EntityType>();
    registry.register_component<cpnt::BossHitbox>();
    // Net
    registry.register_component<cpnt::Replicated>();

    // Sim (order aligned with solo flow)
    engine_ctx.add_system<cpnt::Player>(sys::server_update_player_entities_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Bullet>(sys::server_bullet_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::BulletShooter>(sys::server_bullet_shooter_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Bullet, cpnt::BulletShooter, cpnt::Enemy, cpnt::Shooter, cpnt::Health,
                          cpnt::Hitbox, cpnt::Player, cpnt::BossHitbox>(sys::server_collision_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::MovementPattern, cpnt::Velocity>(sys::server_enemy_movement_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Enemy, cpnt::Health>(sys::server_enemy_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Player, cpnt::Velocity>(sys::server_player_control_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::MovementPattern, cpnt::Velocity, cpnt::Shooter, cpnt::Player>(
        sys::server_shooter_movement_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Health, cpnt::Shooter, cpnt::Player>(sys::server_shooter_system);
    engine_ctx.add_system<cpnt::Stats>(sys::server_stat_system);
    engine_ctx.add_system<cpnt::Replicated>(sys::create_snapshot_system);
    engine_ctx.add_system<>(sys::update_snapshots_system);
    engine_ctx.add_system<cpnt::Replicated>(sys::send_snapshot_to_client_system);
    engine_ctx.add_system<>(sys::clear_tombstones_system);


    constexpr float k_dist_min = 0.1f;
    constexpr float k_dist_max = 0.8f;

    constexpr float k_enemy_sprite_width = 21.0f;
    constexpr float k_enemy_sprite_height = 23.0f;
    constexpr float k_enemy_scale = 5.0f;
    constexpr float k_enemy_hitbox_width = 15.0f;
    constexpr float k_enemy_hitbox_height = 18.0f;
    constexpr float k_pattern_base_speed = 201.0f;

    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
    const int k_width = static_cast<int>(engine_ctx.window_size.x);
    const int k_height = static_cast<int>(engine_ctx.window_size.y);
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(k_dist_min, k_dist_max);

    auto stats = registry.spawn_entity();
    registry.add_component(stats, cpnt::Replicated{static_cast<std::uint32_t>(stats)});
    registry.add_component(stats, cpnt::Stats{0, 0, 0, 0});

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

    // Create shooters
    constexpr float k_shooter_sprite_width = 22.0f;
    constexpr float k_shooter_sprite_height = 18.0f;
    constexpr float k_shooter_scale = 5.0f;
    constexpr float k_shooter_hitbox_width = 15.0f;
    constexpr float k_shooter_hitbox_height = 18.0f;

    for (size_t i = 0; i < engine_ctx.k_max_shooter; i++) {
        auto shooter = engine_ctx.registry.spawn_entity();

        float spawn_y = (float)GetRandomValue(engine_ctx.k_spawn_margin, k_height - engine_ctx.k_spawn_margin);
        float spawn_x = (float)GetRandomValue(k_width, k_width * 2);

        engine_ctx.registry.add_component(shooter, cpnt::Replicated{static_cast<std::uint32_t>(shooter)});
        engine_ctx.registry.add_component(shooter, cpnt::EntityType{"shooter"});

        engine_ctx.registry.add_component(shooter, engn::cpnt::Transform{spawn_x, spawn_y, 0, 55.f, 45.f, 0, 1, 1, 1}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
        engine_ctx.registry.add_component(
            shooter, cpnt::Velocity{-(engine_ctx.k_shooter_base_speed + randf() * engine_ctx.k_shooter_speed_variance), 0.0f, 0.0f, 0.0f, 0.0f});
        engine_ctx.registry.add_component(shooter, cpnt::Shooter{0.0f});
        engine_ctx.registry.add_component(shooter, cpnt::Health{engine_ctx.k_shooter_health, engine_ctx.k_shooter_health});

        cpnt::MovementPattern pat;
        pat.speed = k_pattern_base_speed + randf() * engine_ctx.k_pattern_speed_variance;
        pat.amplitude = (float)GetRandomValue(1, engine_ctx.k_pattern_amplitude_max);
        pat.frequency = dist(gen);
        pat.timer = 1.f;
        pat.type = cpnt::MovementPattern::PatternType::Straight;
        pat.base_y = spawn_y;

        engine_ctx.registry.add_component(shooter, std::move(pat));
        engine_ctx.registry.add_component(shooter, cpnt::Hitbox{k_shooter_hitbox_width * (k_shooter_scale + 2),
                                                                k_shooter_hitbox_height * (k_shooter_scale + 2),
                                                                -k_shooter_sprite_width * 2, -k_shooter_sprite_height * 3});
    }
}
