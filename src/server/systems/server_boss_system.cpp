#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"

using namespace engn;

namespace {
constexpr int k_level_to_appear = 4;
constexpr float k_roar_thickness = 20.0f;
constexpr float k_cooldown_1_duration = 3.0f;
constexpr float k_cooldown_2_duration = 5.0f;

constexpr float k_bullet_sprite_x = 249.f;
constexpr float k_bullet_sprite_y = 105.0f;
constexpr float k_bullet_width = 16.0f;
constexpr float k_bullet_height = 8.0f;
constexpr float k_bullet_scale = 1.0f;
constexpr float k_bullet_speed = 300.0f;
} // namespace

void sys::server_boss_system(
    EngineContext& ctx, ecs::SparseArray<cpnt::Boss> const& boss, ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::Stats> const& stats, ecs::SparseArray<cpnt::BossHitbox> const& boss_hitboxes,
    ecs::SparseArray<cpnt::Enemy> const& enemies, ecs::SparseArray<cpnt::Shooter> const& shooters,
    ecs::SparseArray<cpnt::BulletShooter> const& bullets_shooter, ecs::SparseArray<cpnt::Bullet> const& bullets,
    ecs::SparseArray<cpnt::Health> const& healths) {
    std::vector<ecs::Entity> entity_to_kill;
    auto& reg = ctx.registry;
    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
    const int k_width = static_cast<int>(ctx.window_size.x);
    const int k_height = static_cast<int>(ctx.window_size.y);
    // NOLINTEND(cppcoreguidelines-pro-type-union-access)
    const float k_max_dist = sqrtf(
        k_width * k_width + k_height * k_height); // NOLINT(cppcoreguidelines-narrowing-conversions,-warnings-as-errors)

    // Check level and spawn boss if needed
    for (auto [stats_idx, stats_opt] : ecs::indexed_zipper(stats)) {
        if (stats_opt) {
            auto& stat = reg.get_components<cpnt::Stats>()[stats_idx];
            if (stat->level >= k_level_to_appear && stat->level % k_level_to_appear == 0 &&
                stat->boss_active == false) {
                stat->boss_active = true;
                const float k_boss_sprite_x = 27.0f;
                const float k_boss_sprite_y = 861.0f;
                const float k_boss_sprite_width = 154.0f;
                const float k_boss_sprite_height = 203.0f;
                const float k_boss_scale = 5.0f;
                const float k_boss_hitbox_head_width = 111.f * k_boss_scale;
                const float k_boss_hitbox_head_height = 86.f * k_boss_scale;
                const float k_boss_hitbox_body_width = 76.f * k_boss_scale;
                const float k_boss_hitbox_body_height = 93.f * k_boss_scale;
                const float k_boss_hitbox_tail_width = 68.f * k_boss_scale;
                const float k_boss_hitbox_tail_height = 30.f * k_boss_scale;
                const float k_boss_hitbox_head_x_offset = 0.f;
                const float k_boss_hitbox_head_y_offset = 0.f;
                const float k_boss_hitbox_body_x_offset = 83.f * k_boss_scale;
                const float k_boss_hitbox_body_y_offset = 85.f * k_boss_scale;
                const float k_boss_hitbox_tail_x_offset = 55.f * k_boss_scale;
                const float k_boss_hitbox_tail_y_offset = 176.f * k_boss_scale;

                auto boss_entity = ctx.registry.spawn_entity();
                ctx.registry.add_component(boss_entity, cpnt::Replicated{static_cast<std::uint32_t>(boss_entity)});
                ctx.registry.add_component(
                    boss_entity,
                    cpnt::Transform{ctx.window_size.x - 400.f, 0.f, 0, 0, 0, 0, 1, 1,
                                    1}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors,
                                         // cppcoreguidelines-pro-type-union-access)
                ctx.registry.add_component(
                    boss_entity,
                    cpnt::Boss{0.f,
                               0.f,
                               0.f,
                               true,
                               false,
                               {1350.f, 400.f},
                               0.f,
                               600.f}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                ctx.registry.add_component(
                    boss_entity,
                    cpnt::Health{100, 100}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                ctx.registry.add_component(boss_entity, cpnt::Velocity{0.f, 0.f, 0.f, 0.f, 0.f, 0.f});
                ctx.registry.add_component(boss_entity,
                                           cpnt::BossHitbox{k_boss_hitbox_head_width, k_boss_hitbox_head_height,
                                                            k_boss_hitbox_head_x_offset, k_boss_hitbox_head_y_offset,
                                                            k_boss_hitbox_body_width, k_boss_hitbox_body_height,
                                                            k_boss_hitbox_body_x_offset, k_boss_hitbox_body_y_offset,
                                                            k_boss_hitbox_tail_width, k_boss_hitbox_tail_height,
                                                            k_boss_hitbox_tail_x_offset, k_boss_hitbox_tail_y_offset});

                // Mark the stats entity as dirty so the change is replicated
                reg.mark_dirty<cpnt::Stats>(reg.entity_from_index(stats_idx));
            }

            // Handle roar wave attack
            for (auto [boss_idx, boss_opt] : ecs::indexed_zipper(boss)) {
                if (boss_opt) {
                    auto& boss_comp = reg.get_components<cpnt::Boss>()[boss_idx];

                    if (boss_comp->time_to_roar || boss_comp->roar_active) {
                        boss_comp->time_to_roar = false;
                        if (!boss_comp->roar_active) {
                            boss_comp->roar_active = true;
                            boss_comp->wave_radius = 0.0f;
                        }

                        // UPDATE wave radius
                        if (boss_comp->roar_active) {
                            boss_comp->wave_radius += boss_comp->wave_speed * ctx.delta_time;

                            if (boss_comp->wave_radius > k_max_dist) {
                                boss_comp->roar_active = false;
                            }
                        }

                        // Destroy enemies and bullets in wave radius
                        for (auto [pos_idx, pos_opt, charg_opt, shot_opt, bul_shot_opt, bul_opt] :
                             ecs::indexed_zipper(positions, enemies, shooters, bullets_shooter, bullets)) {
                            if (pos_opt && (charg_opt || shot_opt || bul_shot_opt || bul_opt)) {
                                auto& pos = reg.get_components<cpnt::Transform>()[pos_idx];

                                float dist_x = pos->x - boss_comp->wave_center.x;
                                float dist_y = pos->y - boss_comp->wave_center.y;
                                float distance = sqrtf(dist_x * dist_x + dist_y * dist_y);

                                if (distance >= boss_comp->wave_radius - k_roar_thickness &&
                                    distance <= boss_comp->wave_radius) {
                                    entity_to_kill.push_back(reg.entity_from_index(pos_idx));
                                }
                            }
                        }

                        // Cleanup: kill any remaining enemies/bullets when wave is complete
                        if (boss_comp->wave_radius > k_max_dist) {
                            boss_comp->roar_active = false;

                            auto& positions_cleanup = reg.get_components<cpnt::Transform>();
                            auto& enemies_cleanup = reg.get_components<cpnt::Enemy>();
                            auto& shooters_cleanup = reg.get_components<cpnt::Shooter>();
                            auto& bullets_cleanup = reg.get_components<cpnt::Bullet>();
                            auto& bullets_shooter_cleanup = reg.get_components<cpnt::BulletShooter>();

                            // Kill all chargers
                            for (auto [idx, pos_opt, enemy_opt] :
                                 ecs::indexed_zipper(positions_cleanup, enemies_cleanup)) {
                                if (pos_opt && enemy_opt) {
                                    entity_to_kill.push_back(reg.entity_from_index(idx));
                                }
                            }

                            // Kill all shooters
                            for (auto [idx, pos_opt, shooter_opt] :
                                 ecs::indexed_zipper(positions_cleanup, shooters_cleanup)) {
                                if (pos_opt && shooter_opt) {
                                    entity_to_kill.push_back(reg.entity_from_index(idx));
                                }
                            }

                            // Kill all bullets
                            for (auto [idx, pos_opt, bullet_opt] :
                                 ecs::indexed_zipper(positions_cleanup, bullets_cleanup)) {
                                if (pos_opt && bullet_opt) {
                                    entity_to_kill.push_back(reg.entity_from_index(idx));
                                }
                            }

                            // Kill all Shooter bullets
                            for (auto [idx, pos_opt, bul_shot_opt] :
                                 ecs::indexed_zipper(positions_cleanup, bullets_shooter_cleanup)) {
                                if (pos_opt && bul_shot_opt) {
                                    entity_to_kill.push_back(reg.entity_from_index(idx));
                                }
                            }
                        }

                        // Mark boss as dirty to replicate roar state
                        reg.mark_dirty<cpnt::Boss>(reg.entity_from_index(boss_idx));
                    }
                }
            }
        }
    }

    // Check Health and remove boss if dead
    for (auto [boss_idx, boss_tag_opt, health_opt] : ecs::indexed_zipper(boss, healths)) {
        if (boss_tag_opt && health_opt) {
            if (health_opt->hp <= 0) {
                entity_to_kill.push_back(reg.entity_from_index(boss_idx));
            }
        }
    }

    // Kill all marked entities
    for (auto e : entity_to_kill) {
        reg.kill_entity(e);
    }

    // Boss attacks (when not roaring)
    for (auto [boss_idx, boss_opt, pos_opt] : ecs::indexed_zipper(boss, positions)) {
        if (boss_opt && pos_opt && !boss_opt->roar_active) {
            auto& boss_comp = reg.get_components<cpnt::Boss>()[boss_idx];
            auto& pos = reg.get_components<cpnt::Transform>()[boss_idx];

            boss_comp->cooldown_1 -= ctx.delta_time;
            boss_comp->cooldown_2 -= ctx.delta_time;

            bool boss_updated = false;

            // Attack 1: Semi-circle Bullet spray from boss position
            if (boss_comp->cooldown_1 <= 0.0f) {
                boss_comp->cooldown_1 = k_cooldown_1_duration;
                boss_updated = true;

                // Spawn point (adjust based on your boss sprite)
                float spawn_x = pos->x + 400.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                float spawn_y = pos->y + 550.0f; // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)

                // Create semi-circle of bullets (180 degrees, facing left/down)
                constexpr int k_num_bullets = 12;
                constexpr float k_start_angle = 90.0f; // degrees
                constexpr float k_end_angle = 270.0f;

                for (int i = 0; i < k_num_bullets; i++) {
                    float angle =
                        k_start_angle +
                        (k_end_angle - k_start_angle) * i /
                            (k_num_bullets - 1); // NOLINT(cppcoreguidelines-narrowing-conversions,-warnings-as-errors)
                    float rad = angle * DEG2RAD;

                    float vx = cosf(rad) * k_bullet_speed;
                    float vy = sinf(rad) * k_bullet_speed;

                    auto bullet = reg.spawn_entity();
                    reg.add_component(bullet, cpnt::Replicated{static_cast<std::uint32_t>(bullet)});
                    reg.add_component(
                        bullet,
                        cpnt::Transform{spawn_x, spawn_y, 0.0f, 8.0f, 4.0f, 0.0f, 1.0f, 1.0f,
                                        1.0f}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                    reg.add_component(bullet, cpnt::Velocity{vx, vy, angle, 0.0f, 0.0f, 0.0f});
                    reg.add_component(bullet, cpnt::BulletShooter{});
                    reg.add_component(
                        bullet, cpnt::Hitbox{16.0f, 8.0f, 0.f,
                                             0.f}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                }
            }

            // Attack 2: Vertical wall of bullets from right side moving left
            if (boss_comp->cooldown_2 <= 0.0f) {
                boss_comp->cooldown_2 = k_cooldown_2_duration;
                boss_updated = true;

                const int k_height_int =
                    static_cast<int>(ctx.window_size.y); // NOLINT(cppcoreguidelines-pro-type-union-access)
                const int k_width_int =
                    static_cast<int>(ctx.window_size.x); // NOLINT(cppcoreguidelines-pro-type-union-access)

                // Spawn bullets along right edge
                constexpr int k_num_bullets = 25;
                constexpr float k_spacing = 100.0f; // Vertical spacing between bullets

                float start_y =
                    (k_height_int - (k_num_bullets * k_spacing)) /
                    2.0f; // Center vertically // NOLINT(cppcoreguidelines-narrowing-conversions,-warnings-as-errors,
                          // cppcoreguidelines-avoid-magic-numbers)

                for (int i = 0; i < k_num_bullets; i++) {
                    float spawn_x =
                        k_width_int - 1.0f; // Just off right edge //
                                            // NOLINT(cppcoreguidelines-narrowing-conversions,-warnings-as-errors)
                    float spawn_y =
                        start_y + i * k_spacing; // NOLINT(cppcoreguidelines-narrowing-conversions,-warnings-as-errors)

                    float vx = -k_bullet_speed; // Move left
                    float vy = 0.0f;

                    auto bullet = reg.spawn_entity();
                    reg.add_component(bullet, cpnt::Replicated{static_cast<std::uint32_t>(bullet)});
                    reg.add_component(
                        bullet,
                        cpnt::Transform{spawn_x, spawn_y, 0.0f, 8.0f, 4.0f, 0.0f, 1.0f, 1.0f,
                                        1.0f}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                    reg.add_component(
                        bullet,
                        cpnt::Velocity{vx, vy, 180.0f, 0.0f, 0.0f,
                                       0.0f}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                    reg.add_component(bullet, cpnt::BulletShooter{});
                    reg.add_component(
                        bullet, cpnt::Hitbox{16.0f, 8.0f, 0.f,
                                             0.f}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                }
            }

            // Mark boss as dirty if cooldowns were updated
            if (boss_updated) {
                reg.mark_dirty<cpnt::Boss>(reg.entity_from_index(boss_idx));
            }
        }
    }
}
