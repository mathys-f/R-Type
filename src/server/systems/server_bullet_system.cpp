#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "systems/systems.h"

using namespace engn;

void sys::server_bullet_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                        ecs::SparseArray<cpnt::Velocity> const& velocities,
                        ecs::SparseArray<cpnt::Bullet> const& bullets) {
    std::vector<ecs::Entity> to_kill;
    auto& reg = ctx.registry;
    float dt = ctx.delta_time;

    for (auto [idx, pos_opt, vel_opt, bullet_opt] : ecs::indexed_zipper(positions, velocities, bullets)) {
        if (pos_opt && vel_opt && bullet_opt) {
            auto entity = reg.entity_from_index(idx);
            auto& pos = reg.get_components<cpnt::Transform>()[idx];

            if (pos && vel_opt) {
                reg.mark_dirty<cpnt::Transform>(entity);
                pos->x += vel_opt->vx * dt;
                pos->y += vel_opt->vy * dt;

                // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
                // Extended bounds check: allow bullets to travel further off-screen
                // This gives clients more time to receive and render bullets before destruction
                constexpr float k_offscreen_margin = 400.0f;
                if (pos->x > ctx.window_size.x + k_offscreen_margin || 
                    pos->x < -k_offscreen_margin || 
                    pos->y > ctx.window_size.y + k_offscreen_margin || 
                    pos->y < -k_offscreen_margin) {
                    to_kill.push_back(entity);
                }
                // NOLINTEND(cppcoreguidelines-pro-type-union-access)
            }
        }
    }

    for (auto e : to_kill) {
        reg.kill_entity_deferred(e);
    }
}
