#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"

using namespace engn;

namespace {
constexpr float k_offscreen_left = -100.0f;
constexpr int k_spawn_margin = 100;
constexpr float k_spawn_multiplier = 1.5f;
} // namespace

void sys::server_enemy_system(EngineContext& ctx,
    ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::Velocity> const& velocities,
    ecs::SparseArray<cpnt::Enemy> const& enemies,
    ecs::SparseArray<cpnt::Health> const& healths) {
    // LOG_DEBUG("Running server_enemy_system");
    auto& reg = ctx.registry;

    for (auto [idx, pos_opt, vel_opt, enemy_opt, health_opt] :
         ecs::indexed_zipper(positions, velocities, enemies, healths)) {
        if (pos_opt && vel_opt && enemy_opt && health_opt) {
            auto& pos = reg.get_components<cpnt::Transform>()[idx];
            auto& health = reg.get_components<cpnt::Health>()[idx];
            auto& vel = reg.get_components<cpnt::Velocity>()[idx];
            const auto k_entity = reg.entity_from_index(idx);

            if (health && health->hp <= 0) {
                const std::uint32_t k_entity_id = static_cast<std::uint32_t>(k_entity);
                if (ctx.dead_enemy_ids.insert(k_entity_id).second) {
                    reg.mark_dirty<cpnt::Health>(k_entity);
                    continue;
                }
                ctx.dead_enemy_ids.erase(k_entity_id);
                // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
                pos->x =
                    static_cast<float>(GetRandomValue(static_cast<int>(ctx.window_size.x),
                                                      static_cast<int>(ctx.window_size.x * k_spawn_multiplier)));
                pos->y = static_cast<float>(
                    GetRandomValue(k_spawn_margin, static_cast<int>(ctx.window_size.y) - k_spawn_margin));
                // NOLINTEND(cppcoreguidelines-pro-type-union-access)
                health->hp = health->max_hp;
                reg.mark_dirty<cpnt::Health>(k_entity);
                reg.mark_dirty<cpnt::Transform>(k_entity);
                continue;
            }

            if (pos && vel_opt) {
                reg.mark_dirty<cpnt::Transform>(k_entity);
                pos->x += vel_opt->vx;
                pos->y += vel_opt->vy;
                // Respawn if off screen
                if (pos->x < k_offscreen_left) {
                    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
                    pos->x =
                        static_cast<float>(GetRandomValue(static_cast<int>(ctx.window_size.x),
                                                          static_cast<int>(ctx.window_size.x * k_spawn_multiplier)));
                    pos->y = static_cast<float>(
                        GetRandomValue(k_spawn_margin, static_cast<int>(ctx.window_size.y) - k_spawn_margin));
                    // NOLINTEND(cppcoreguidelines-pro-type-union-access)
                    reg.mark_dirty<cpnt::Transform>(k_entity);
                }
            }
        }
    }
}
