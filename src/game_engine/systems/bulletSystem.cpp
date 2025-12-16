#include "components/components.h"
#include "engine.h"
#include "systems/systems.h"
#include "ecs/zipper.h"

using namespace engn;

void sys::BulletSystem(EngineContext &ctx,
                 ecs::SparseArray<cpnt::Transform> const& positions,
                 ecs::SparseArray<cpnt::Velocity> const& velocities,
                 ecs::SparseArray<cpnt::Bullet> const& bullets)
{
    std::vector<ecs::Entity> to_kill;
    auto& reg = ctx.registry;
    float dt = ctx.delta_time;
    
    for (auto [idx, pos_opt, vel_opt, bullet_opt] : ecs::indexed_zipper(positions, velocities, bullets)) {
        if (pos_opt && vel_opt && bullet_opt) {
            auto entity = reg.entity_from_index(idx);
            auto& pos = reg.get_components<cpnt::Transform>()[idx];
            
            if (pos && vel_opt) {
                pos->x += vel_opt->vx * dt;
                pos->y += vel_opt->vy * dt;
                
                if (pos->x > ctx.window_size.x || pos->x < 0 || pos->y > ctx.window_size.y || pos->y < 0) {
                    to_kill.push_back(entity);
                }
            }
        }
    }
    
    for (auto e : to_kill) {
        reg.kill_entity(e);
    }
}
