#include "../components.h"
#include "../../ecs/registry.h"
#include "../../ecs/zipper.h"

void CollisionSystem(ecs::Registry& reg,
                    ecs::SparseArray<Position> const& positions,
                    ecs::SparseArray<Bullet> const& bullets,
                    ecs::SparseArray<Enemy> const& enemies,
                    ecs::SparseArray<Health> const& healths) {
    std::vector<ecs::Entity> bullets_to_kill;
    
    // Get bullet entities
    for (auto [bullet_idx, bullet_pos_opt, bullet_tag_opt] : 
         ecs::indexed_zipper(positions, bullets)) {
        if (bullet_pos_opt && bullet_tag_opt) {
            // Check against all enemies
            for (auto [enemy_idx, enemy_pos_opt, enemy_tag_opt, health_opt] : 
                 ecs::indexed_zipper(positions, enemies, healths)) {
                if (enemy_pos_opt && enemy_tag_opt && health_opt && health_opt->current > 0) {
                    Rectangle enemyRect = {enemy_pos_opt->x, enemy_pos_opt->y, 100, 100};
                    Vector2 bulletPos = {bullet_pos_opt->x, bullet_pos_opt->y};
                    
                    if (CheckCollisionCircleRec(bulletPos, 5, enemyRect)) {
                        bullets_to_kill.push_back(reg.entity_from_index(bullet_idx));
                        
                        auto& health = reg.get_components<Health>()[enemy_idx];
                        if (health) {
                            health->current--;
                        }
                        break;
                    }
                }
            }
        }
    }
    
    for (auto e : bullets_to_kill) {
        reg.kill_entity(e);
    }
}