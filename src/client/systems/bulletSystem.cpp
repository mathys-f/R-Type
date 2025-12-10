#include "../components.h"
#include "../../ecs/registry.h"
#include "../../ecs/zipper.h"

void BulletSystem(ecs::Registry& reg,
                 ecs::SparseArray<Position> const& positions,
                 ecs::SparseArray<Velocity> const& velocities,
                 ecs::SparseArray<Bullet> const& bullets,
                 float dt) {
    std::vector<ecs::Entity> to_kill;
    
    for (auto [idx, pos_opt, vel_opt, bullet_opt] : ecs::indexed_zipper(positions, velocities, bullets)) {
        if (pos_opt && vel_opt && bullet_opt) {
            auto entity = reg.entity_from_index(idx);
            auto& pos = reg.get_components<Position>()[idx];
            
            if (pos && vel_opt) {
                pos->x += vel_opt->x * dt;
                pos->y += vel_opt->y * dt;
                
                if (pos->x > WIDTH || pos->x < 0 || pos->y > HEIGHT || pos->y < 0) {
                    to_kill.push_back(entity);
                }
            }
        }
    }
    
    for (auto e : to_kill) {
        reg.kill_entity(e);
    }
}