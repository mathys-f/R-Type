#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "systems/systems.h"
#include "raylib.h"

#include <cmath>
#include <limits>

using namespace engn;

namespace {
constexpr float k_offscreen_left = -100.0f;
constexpr int k_spawn_margin = 100;
constexpr float k_spawn_multiplier = 1.5f;
constexpr float k_shoot_interval = 1.0f;
constexpr float k_rotation_offset = 180.0f;

// Bullet (shooter)
constexpr float k_bullet_speed = 400.0f;
constexpr float k_bullet_width = 16.0f;
constexpr float k_bullet_height = 8.0f;
constexpr float k_bullet_origin_x = k_bullet_width / 2.0f;
constexpr float k_bullet_origin_y = k_bullet_height / 2.0f;
} // namespace

void sys::server_shooter_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                                ecs::SparseArray<cpnt::Velocity> const& velocities,
                                ecs::SparseArray<cpnt::Health> const& healths,
                                ecs::SparseArray<cpnt::Shooter> const& shooters,
                                ecs::SparseArray<cpnt::Player> const& players) {
    auto& reg = ctx.registry;
    float dt = ctx.delta_time;

    for (auto [idx, pos_opt, vel_opt, shot_opt, health_opt] :
         ecs::indexed_zipper(positions, velocities, shooters, healths)) {
        if (!pos_opt || !vel_opt || !shot_opt || !health_opt) {
            continue;
        }

        auto entity = reg.entity_from_index(idx);
        auto& pos = reg.get_components<cpnt::Transform>()[idx];
        auto& health = reg.get_components<cpnt::Health>()[idx];
        auto& vel = reg.get_components<cpnt::Velocity>()[idx];
        auto& shot = reg.get_components<cpnt::Shooter>()[idx];

        if (health && health->hp <= 0) {
            const std::uint32_t k_entity_id = static_cast<std::uint32_t>(entity);
            if (ctx.dead_shooter_ids.insert(k_entity_id).second) {
                reg.mark_dirty<cpnt::Health>(entity);
                continue;
            }
            ctx.dead_shooter_ids.erase(k_entity_id);
            // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
            pos->x =
                static_cast<float>(GetRandomValue(static_cast<int>(ctx.window_size.x),
                                                  static_cast<int>(ctx.window_size.x * k_spawn_multiplier)));
            pos->y = static_cast<float>(
                GetRandomValue(k_spawn_margin, static_cast<int>(ctx.window_size.y) - k_spawn_margin));
            // NOLINTEND(cppcoreguidelines-pro-type-union-access)
            health->hp = health->max_hp;
            reg.mark_dirty<cpnt::Health>(entity);
            reg.mark_dirty<cpnt::Transform>(entity);
            continue;
        }

        if (pos && vel_opt) {
            reg.mark_dirty<cpnt::Transform>(entity);
            pos->x += vel_opt->vx;
            pos->y += vel_opt->vy;

            if (pos->x < k_offscreen_left) {
                // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
                pos->x =
                    static_cast<float>(GetRandomValue(static_cast<int>(ctx.window_size.x),
                                                      static_cast<int>(ctx.window_size.x * k_spawn_multiplier)));
                pos->y = static_cast<float>(
                    GetRandomValue(k_spawn_margin, static_cast<int>(ctx.window_size.y) - k_spawn_margin));
                // NOLINTEND(cppcoreguidelines-pro-type-union-access)
                reg.mark_dirty<cpnt::Transform>(entity);
            }
        }

        shot->timer += dt;
        if (shot->timer >= k_shoot_interval && pos_opt->x < ctx.window_size.x) { // NOLINT(cppcoreguidelines-pro-type-union-access)
            float dx = 0.0f;
            float dy = 0.0f;
            for (auto [pidx, ppos_opt, pplay_opt] : ecs::indexed_zipper(positions, players)) {
                if (!ppos_opt || !pplay_opt) {
                    continue;
                }
                auto& ppos = reg.get_components<cpnt::Transform>()[pidx];
                dx = ppos->x - pos->x;
                dy = ppos->y - pos->y;
                break;
            }

            const float k_length = std::sqrt((dx * dx) + (dy * dy));
            if (k_length > std::numeric_limits<float>::epsilon()) {
                const float k_dir_x = dx / k_length;
                const float k_dir_y = dy / k_length;
                const float k_vx = k_dir_x * k_bullet_speed;
                const float k_vy = k_dir_y * k_bullet_speed;

                auto bullet = reg.spawn_entity();
                reg.add_component(bullet, cpnt::Replicated{static_cast<std::uint32_t>(bullet)});
                reg.add_component(bullet, cpnt::Transform{pos->x, pos->y, 0.0f, k_bullet_origin_x, k_bullet_origin_y, 0.0f,
                                                          1.0f, 1.0f, 1.0f});
                reg.add_component(bullet, cpnt::Velocity{k_vx, k_vy, vel->vz + k_rotation_offset, 0.0f, 0.0f, 0.0f});
                reg.add_component(bullet, cpnt::BulletShooter{});
                reg.add_component(bullet, cpnt::Hitbox{k_bullet_width, k_bullet_height, 0.0f, 0.0f});
            }
            shot->timer = 0.0f;
        }
    }
}
