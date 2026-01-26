#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"

#include <random>

using namespace engn;

namespace {
constexpr int k_rand_range = 1000;
constexpr float k_rand_divisor = 1000.0f;
constexpr int k_new_charger_enemies = 1;
constexpr int k_new_shooter_enemies = 1;
constexpr int k_boss_level = 4;
constexpr float k_dist_min = 0.1f;
constexpr float k_dist_max = 0.8f;

constexpr float k_enemy_sprite_width = 21.0f;
constexpr float k_enemy_sprite_height = 23.0f;
constexpr float k_enemy_scale = 5.0f;
constexpr float k_enemy_hitbox_width = 15.0f;
constexpr float k_enemy_hitbox_height = 18.0f;
constexpr float k_pattern_base_speed = 201.0f;
constexpr float k_spawn_multiplier = 1.5f;

constexpr float k_shooter_sprite_width = 22.0f;
constexpr float k_shooter_sprite_height = 18.0f;
constexpr float k_shooter_scale = 5.0f;
constexpr float k_shooter_hitbox_width = 15.0f;
constexpr float k_shooter_hitbox_height = 18.0f;
constexpr float k_shooter_hitbox_scale_extra = 2.0f;
constexpr float k_shooter_hitbox_offset_multiplier = 2.0f;
constexpr float k_shooter_hitbox_offset_height_multiplier = 3.0f;
} // namespace

static float randf() {
    return static_cast<float>(rand() % k_rand_range) / k_rand_divisor; // NOLINT(clang-analyzer-security.insecureAPI.rand)
}

void sys::server_stat_system(EngineContext& ctx, ecs::SparseArray<cpnt::Stats> const& stats) {
    auto& reg = ctx.registry;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(k_dist_min, k_dist_max);
    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
    const int k_width = static_cast<int>(ctx.window_size.x);
    const int k_height = static_cast<int>(ctx.window_size.y);
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)

    for (auto [stats_idx, stats_opt] : ecs::indexed_zipper(stats)) {
        if (!stats_opt) {
            continue;
        }
        auto& stat = reg.get_components<cpnt::Stats>()[stats_idx];
        if (!stat) {
            continue;
        }

        bool updated = false;
        if (stat->score >= stat->point_to_next_level) {
            stat->level += 1;
            stat->point_to_next_level += 500; // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
            updated = true;

            if (stat->level % k_boss_level != 0) {
                for (size_t i = 0; i < static_cast<size_t>(k_new_charger_enemies * stat->level); i++) {
                    auto enemy = reg.spawn_entity();

                    float spawn_y = static_cast<float>(GetRandomValue(ctx.k_spawn_margin,
                                                                      k_height - ctx.k_spawn_margin));
                    float spawn_x = static_cast<float>(
                        GetRandomValue(k_width,
                                       static_cast<int>(static_cast<float>(k_width) * k_spawn_multiplier)));

                    reg.add_component(enemy, cpnt::Replicated{static_cast<std::uint32_t>(enemy)});
                    reg.add_component(enemy, cpnt::EntityType{"charger"});
                    reg.add_component(enemy, cpnt::Transform{spawn_x, spawn_y, 0, 0, 0, 0, 1, 1, 1});
                    reg.add_component(
                        enemy, cpnt::Velocity{-(ctx.k_enemy_base_speed + randf() * ctx.k_enemy_speed_variance), 0.0f, 0.0f, 0.0f, 0.0f});
                    reg.add_component(enemy, cpnt::Enemy{});
                    reg.add_component(enemy, cpnt::Health{ctx.k_enemy_health, ctx.k_enemy_health});

                    cpnt::MovementPattern pat;
                    pat.speed = k_pattern_base_speed + randf() * ctx.k_pattern_speed_variance;
                    pat.amplitude = static_cast<float>(GetRandomValue(1, ctx.k_pattern_amplitude_max));
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

                    reg.add_component(enemy, std::move(pat));
                    reg.add_component(enemy, cpnt::Hitbox{k_enemy_hitbox_width * k_enemy_scale,
                                                          k_enemy_hitbox_height * k_enemy_scale,
                                                          k_enemy_sprite_width, k_enemy_sprite_height});
                }

                for (size_t i = 0; i < static_cast<size_t>(k_new_shooter_enemies); i++) {
                    auto shooter = reg.spawn_entity();

                    float spawn_y = static_cast<float>(GetRandomValue(ctx.k_spawn_margin,
                                                                      k_height - ctx.k_spawn_margin));
                    float spawn_x = static_cast<float>(
                        GetRandomValue(k_width,
                                       static_cast<int>(static_cast<float>(k_width) * k_spawn_multiplier)));

                    reg.add_component(shooter, cpnt::Replicated{static_cast<std::uint32_t>(shooter)});
                    reg.add_component(shooter, cpnt::EntityType{"shooter"});
                    reg.add_component(shooter, cpnt::Transform{spawn_x, spawn_y, 0, 55.f, 45.f, 0, 1, 1, 1}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                    reg.add_component(
                        shooter, cpnt::Velocity{-(ctx.k_shooter_base_speed + randf() * ctx.k_shooter_speed_variance), 0.0f, 0.0f, 0.0f, 0.0f});
                    reg.add_component(shooter, cpnt::Shooter{0.0f});
                    reg.add_component(shooter, cpnt::Health{ctx.k_shooter_health, ctx.k_shooter_health});

                    cpnt::MovementPattern pat;
                    pat.speed = k_pattern_base_speed + randf() * ctx.k_pattern_speed_variance;
                    pat.amplitude = static_cast<float>(GetRandomValue(1, ctx.k_pattern_amplitude_max));
                    pat.frequency = dist(gen);
                    pat.timer = 1.f;
                    pat.type = cpnt::MovementPattern::PatternType::Straight;
                    pat.base_y = spawn_y;

                    reg.add_component(shooter, std::move(pat));
                    reg.add_component(shooter, cpnt::Hitbox{k_shooter_hitbox_width * (k_shooter_scale + k_shooter_hitbox_scale_extra),
                                                            k_shooter_hitbox_height * (k_shooter_scale + k_shooter_hitbox_scale_extra),
                                                            -k_shooter_sprite_width * k_shooter_hitbox_offset_multiplier,
                                                            -k_shooter_sprite_height * k_shooter_hitbox_offset_height_multiplier});
                }
            }
        }

        if (updated) {
            reg.mark_dirty<cpnt::Stats>(reg.entity_from_index(stats_idx));
        }
    }
}
