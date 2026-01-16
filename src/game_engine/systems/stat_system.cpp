#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "systems/systems.h"

#include <random>

using namespace engn;

namespace {
    constexpr int k_rand_range = 1000;
    constexpr float k_rand_divisor = 1000.0f;
    constexpr int new_charger_enemies = 1;
    constexpr int new_shooter_enemies = 1;
    constexpr int boss_level = 4;
}

static float randf() {
    return static_cast<float>(rand() % k_rand_range) / k_rand_divisor;
}

void sys::stat_system(EngineContext& ctx, ecs::SparseArray<cpnt::Stats> const& stats) {
    auto& reg = ctx.registry;

    const int k_width = static_cast<int>(ctx.k_window_size.x);
    const int k_height = static_cast<int>(ctx.k_window_size.y);
    constexpr float k_dist_min = 0.1f;
    constexpr float k_dist_max = 0.8f;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(k_dist_min, k_dist_max);
    // Debug - show stats
    for (auto [stats_idx, stats_opt] : ecs::indexed_zipper(stats)) {
        if (stats_opt) {
            auto& stat = reg.get_components<cpnt::Stats>()[stats_idx];

            if (stat->score >= stat->point_to_next_level) {
                stat->level += 1;
                stat->point_to_next_level += 500; // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)

                // Pass creation every boss level (4 levels)
                if (stat->level % boss_level == 0) {
                    printf("Boss level reached: %d\n", stat->level);
                    continue;
                }
                printf("Level up! New level: %d\n", stat->level);
                // Create New enemies

                constexpr float k_enemy_sprite_x = 5.0f;
                constexpr float k_enemy_sprite_y = 6.0f;
                constexpr float k_enemy_sprite_width = 21.0f;
                constexpr float k_enemy_sprite_height = 23.0f;
                constexpr float k_enemy_scale = 5.0f;
                constexpr float k_enemy_hitbox_width = 15.0f;
                constexpr float k_enemy_hitbox_height = 18.0f;
                constexpr float k_pattern_base_speed = 201.0f;
                for (size_t i = 0; i < new_charger_enemies * stat->level; i++) {
                    auto enemy = ctx.registry.spawn_entity();

                    float spawn_y = (float)GetRandomValue(ctx.k_spawn_margin, k_height - ctx.k_spawn_margin);
                    float spawn_x = (float)GetRandomValue(k_width, k_width * 2);

                    // Position
                    ctx.registry.add_component(enemy, engn::cpnt::Transform{spawn_x, spawn_y, 0, 0, 0, 0, 1, 1, 1});

                    // Velocity
                    ctx.registry.add_component(
                        enemy, cpnt::Velocity{-(ctx.k_enemy_base_speed + randf() * ctx.k_enemy_speed_variance), 0.0f, 0.0f, 0.0f, 0.0f});

                    // Other components
                    ctx.registry.add_component(enemy, cpnt::Enemy{});
                    ctx.registry.add_component(
                        enemy, cpnt::Sprite{{k_enemy_sprite_x, k_enemy_sprite_y, k_enemy_sprite_width, k_enemy_sprite_height},
                                            k_enemy_scale,
                                            0,
                                            "enemy_ship"});
                    ctx.registry.add_component(enemy, cpnt::Health{ctx.k_enemy_health, ctx.k_enemy_health});

                    // Create a **new MovementPattern instance** for this enemy
                    cpnt::MovementPattern pat;
                    pat.speed = k_pattern_base_speed + randf() * ctx.k_pattern_speed_variance;
                    pat.amplitude = (float)GetRandomValue(1, ctx.k_pattern_amplitude_max);
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

                    ctx.registry.add_component(enemy, std::move(pat));
                    ctx.registry.add_component(enemy, cpnt::Hitbox{k_enemy_hitbox_width * k_enemy_scale, k_enemy_hitbox_height * k_enemy_scale,
                                                                          k_enemy_sprite_width, k_enemy_sprite_height});
                }
                // Create shooters
            
                constexpr float k_shooter_sprite_x = 87.0f;
                constexpr float k_shooter_sprite_y = 67.0f;
                constexpr float k_shooter_sprite_width = 22.0f;
                constexpr float k_shooter_sprite_height = 18.0f;
                constexpr float k_shooter_scale = 5.0f;
                constexpr float k_shooter_hitbox_width = 15.0f;
                constexpr float k_shooter_hitbox_height = 18.0f;
            
                for (size_t i = 0; i < new_shooter_enemies; i++) {
                    auto shooter = ctx.registry.spawn_entity();
            
                    float spawn_y = (float)GetRandomValue(ctx.k_spawn_margin, k_height - ctx.k_spawn_margin);
                    float spawn_x = (float)GetRandomValue(k_width, k_width * 2);
            
                    // Position
                    ctx.registry.add_component(shooter, engn::cpnt::Transform{spawn_x, spawn_y, 0, 55.f, 45.f, 0, 1, 1, 1}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
            
                    // Velocity
                    ctx.registry.add_component(
                        shooter, cpnt::Velocity{-(ctx.k_shooter_base_speed + randf() * ctx.k_shooter_speed_variance), 0.0f, 0.0f, 0.0f, 0.0f});
            
                    // Other components
                    ctx.registry.add_component(shooter, cpnt::Shooter{0});
                    ctx.registry.add_component(
                        shooter, cpnt::Sprite{{k_shooter_sprite_x, k_shooter_sprite_y, k_shooter_sprite_width, k_shooter_sprite_height},
                                            k_shooter_scale,
                                            0,
                                            "shooter_sprite"});
                    ctx.registry.add_component(shooter, cpnt::Health{ctx.k_shooter_health, ctx.k_shooter_health});
            
                    // Create a **new MovementPattern instance** for this shooter
                    cpnt::MovementPattern pat;
                    pat.speed = k_pattern_base_speed + randf() * ctx.k_pattern_speed_variance;
                    pat.amplitude = (float)GetRandomValue(1, ctx.k_pattern_amplitude_max);
                    pat.frequency = dist(gen);
                    pat.timer = 1.f;
                    int pattern_nbr = GetRandomValue(0, 3);
                    switch (pattern_nbr) {
                        case 0:
                            pat.type = cpnt::MovementPattern::PatternType::Straight;
                            break;
                        case 1:
                            pat.type = cpnt::MovementPattern::PatternType::Straight;
                            break;
                        case 2:
                            pat.type = cpnt::MovementPattern::PatternType::Straight;
                            break;
                        case 3:
                            pat.type = cpnt::MovementPattern::PatternType::Straight;
                            break;
                    }
                    pat.base_y = spawn_y;
            
                    ctx.registry.add_component(shooter, std::move(pat));
                    ctx.registry.add_component(shooter, cpnt::Hitbox{k_shooter_hitbox_width * (k_shooter_scale + 2), k_shooter_hitbox_height * (k_shooter_scale + 2),
                                                                          -k_shooter_sprite_width * 2, -k_shooter_sprite_height * 3});
                }
            }
            // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
            // NOLINTBEGIN(cppcoreguidelines-pro-type-vararg,-warnings-as-errors)
            DrawText(TextFormat("Score: %d", stat->score), 10, 10, 20, WHITE);
            DrawText(TextFormat("Enemies killed: %d", stat->kills), 10, 40, 20, WHITE);
            DrawText(TextFormat("Level: %d", stat->level), 10, 70, 20, WHITE);
            DrawText(TextFormat("Points to next level: %d", stat->point_to_next_level), 10, 100, 20, WHITE);
            // NOLINTEND(cppcoreguidelines-pro-type-vararg,-warnings-as-errors)
            // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
        }
    }
}
