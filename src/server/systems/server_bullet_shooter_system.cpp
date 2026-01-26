#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "systems/systems.h"

using namespace engn;

void sys::server_bullet_shooter_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                                       ecs::SparseArray<cpnt::Velocity> const& velocities,
                                       ecs::SparseArray<cpnt::BulletShooter> const& bullets_shooter) {
    std::vector<ecs::Entity> to_kill;
    auto& reg = ctx.registry;
    float dt = ctx.delta_time;

    for (auto [idx, pos_opt, vel_opt, bullet_opt] : ecs::indexed_zipper(positions, velocities, bullets_shooter)) {
        if (pos_opt && vel_opt && bullet_opt) {
            auto entity = reg.entity_from_index(idx);
            auto& pos = reg.get_components<cpnt::Transform>()[idx];

            if (pos && vel_opt) {
                reg.mark_dirty<cpnt::Transform>(entity);
                pos->x += vel_opt->vx * dt;
                pos->y += vel_opt->vy * dt;

                // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
                if (pos->x > ctx.window_size.x || pos->x < 0.0f ||
                    pos->y > ctx.window_size.y || pos->y < 0.0f) {
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
