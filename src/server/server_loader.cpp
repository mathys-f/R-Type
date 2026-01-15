#include "game_engine/api/lua.h"
#include "game_engine/components/components.h"
#include "game_engine/engine.h"
#include "game_engine/systems/systems.h"
#include "raylib.h"
#include "scenes_loaders.h"

#include <random>

using namespace engn;

constexpr float k_width = 1080.0f;
constexpr float k_height = 720.0f;

namespace {
constexpr int k_rand_range = 1000;
constexpr float k_rand_divisor = 1000.0f;
} // namespace

static float randf() {
    return static_cast<float>(rand() % k_rand_range) / k_rand_divisor;
}

void load_server_scene(engn::EngineContext& engine_ctx) {
    constexpr float k_dist_min = 0.1f;
    constexpr float k_dist_max = 0.8f;

    auto& registry = engine_ctx.registry;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(k_dist_min, k_dist_max);

    registry.register_component<cpnt::Bullet>();
    registry.register_component<cpnt::Enemy>();
    registry.register_component<cpnt::Health>();
    registry.register_component<cpnt::Hitbox>();
    registry.register_component<cpnt::MovementPattern>();
    registry.register_component<cpnt::Player>();
    registry.register_component<cpnt::Replicated>();
    registry.register_component<cpnt::Stats>();
    registry.register_component<cpnt::Tag>();
    registry.register_component<cpnt::Transform>();

    engine_ctx.add_system<>(sys::log_inputs);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Bullet>(sys::bullet_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Bullet, cpnt::Enemy, cpnt::Health, cpnt::Player, cpnt::Hitbox, cpnt::Bullet_shooter, cpnt::Shooter, cpnt::Stats>(
        sys::collision_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::MovementPattern, cpnt::Velocity>(sys::enemy_movement_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Velocity, cpnt::Enemy, cpnt::Health, cpnt::Sprite>(sys::enemy_system);
    // engine_ctx.add_system<cpnt::Transform, cpnt::Player, cpnt::Sprite, cpnt::Velocity, cpnt::Health>(
    //     sys::player_control_system);

    // Create enemies
    constexpr int k_spawn_margin = 100;
    constexpr float k_enemy_sprite_width = 21.0f;
    constexpr float k_enemy_sprite_height = 23.0f;
    constexpr float k_enemy_scale = 5.0f;
    constexpr float k_enemy_base_speed = 3.0f;
    constexpr float k_enemy_speed_variance = 5.0f;
    constexpr int k_enemy_health = 3;
    constexpr float k_enemy_hitbox_width = 15.0f;
    constexpr float k_enemy_hitbox_height = 18.0f;
    constexpr float k_pattern_base_speed = 201.0f;
    constexpr float k_pattern_speed_variance = 3.0f;
    constexpr int k_pattern_amplitude_max = 10;

    for (size_t i = 0; i < engine_ctx.k_max_charger; i++) {
        auto enemy = engine_ctx.registry.spawn_entity();

        float spawn_y = (float)GetRandomValue(k_spawn_margin, k_height - k_spawn_margin);
        float spawn_x = (float)GetRandomValue(k_width, k_width * 2);

        // Position
        engine_ctx.registry.add_component(enemy, engn::cpnt::Transform{spawn_x, spawn_y, 0, 0, 0, 0, 1, 1, 1});

        // Velocity
        engine_ctx.registry.add_component(
            enemy, cpnt::Velocity{-(k_enemy_base_speed + randf() * k_enemy_speed_variance), 0.0f, 0.0f, 0.0f, 0.0f});

        // Other components
        engine_ctx.registry.add_component(enemy, cpnt::Enemy{});
        engine_ctx.registry.add_component(enemy, cpnt::Health{k_enemy_health, k_enemy_health});

        // Create a **new MovementPattern instance** for this enemy
        cpnt::MovementPattern pat;
        pat.speed = k_pattern_base_speed + randf() * k_pattern_speed_variance;
        pat.amplitude = (float)GetRandomValue(1, k_pattern_amplitude_max);
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
        engine_ctx.registry.add_component(enemy, cpnt::Hitbox{k_enemy_hitbox_width, k_enemy_hitbox_height,
                                                              k_enemy_sprite_width, k_enemy_sprite_height});
    }
}
