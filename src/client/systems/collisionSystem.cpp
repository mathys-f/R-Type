#include "../components.h"
#include "../../ecs/registry.h"
#include "../../ecs/zipper.h"

void CollisionSystem(ecs::Registry& reg,
                    ecs::SparseArray<Position> const& positions,
                    ecs::SparseArray<Bullet> const& bullets,
                    ecs::SparseArray<Enemy> const& enemies,
                    ecs::SparseArray<Health> const& healths,
                    ecs::SparseArray<Player> const& players,
                    Texture2D& bulletExplosionTexture) {
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
                        
                        auto explosion = reg.spawn_entity();
                        reg.add_component(explosion, Position{bullet_pos_opt->x, bullet_pos_opt->y, {0.0f, 0.0f}});
                        reg.add_component(explosion, Sprite{{114.0f, 18.0f, 17.0f, 16.0f}, 3.0f, 0, bulletExplosionTexture});
                        reg.add_component(explosion, Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                        reg.add_component(explosion, Explosion{ExplosionType::Small, 0.0f, 0.08f, 0, 5});

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

    // Player - Enemy collision

    for (auto [player_idx, player_pos_opt, player_tag_opt] : 
         ecs::indexed_zipper(positions, players)) {
        if (player_pos_opt && player_tag_opt) {
            for (auto [enemy_idx, enemy_pos_opt, enemy_tag_opt, health_opt] : 
                 ecs::indexed_zipper(positions, enemies, healths)) {
                if (enemy_pos_opt && enemy_tag_opt && health_opt && health_opt->current > 0) {
                    Rectangle enemyRect = {enemy_pos_opt->x, enemy_pos_opt->y, 100, 100};
                    Vector2 playerPos = {player_pos_opt->x, player_pos_opt->y};
                    
                    if (CheckCollisionCircleRec(playerPos, 20, enemyRect)) {
                        // Handle player hit logic here
                        // For simplicity, just reduce enemy health
                        auto& health = reg.get_components<Health>()[enemy_idx];
                        if (health) {
                            health->current = 0;
                        }

                        auto& health_player = reg.get_components<Health>()[player_idx];
                        if (health_player) {
                            health_player->current -= 10; // Reduce player health
                        }
                    }
                }
            }
        }
    }

}
