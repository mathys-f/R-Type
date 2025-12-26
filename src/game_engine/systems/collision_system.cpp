#include "components/components.h"
#include "engine.h"
#include "ecs/zipper.h"
#include "systems/systems.h"
#include "raylib.h"

using namespace engn;

void sys::collision_system(EngineContext &ctx,
                    ecs::SparseArray<cpnt::Transform> const& positions,
                    ecs::SparseArray<cpnt::Bullet> const& bullets,
                    ecs::SparseArray<cpnt::Enemy> const& enemies,
                    ecs::SparseArray<cpnt::Health> const& healths,
                    ecs::SparseArray<cpnt::Player> const& players,
                    ecs::SparseArray<cpnt::Hitbox> const& hitboxes) {
    std::vector<ecs::Entity> bullets_to_kill;
    auto& reg = ctx.registry;
    
    // Get bullet entities
    for (auto [bullet_idx, bullet_pos_opt, bullet_tag_opt] : 
         ecs::indexed_zipper(positions, bullets)) {
        if (bullet_pos_opt && bullet_tag_opt) {
            // Check against all enemies
            for (auto [enemy_idx, enemy_pos_opt, enemy_tag_opt, health_opt, hitbox_opt] :
                ecs::indexed_zipper(positions, enemies, healths, hitboxes)) {
                if (enemy_pos_opt && enemy_tag_opt && health_opt && health_opt->hp > 0 && hitbox_opt) {
                    Rectangle enemyRect = {
                        enemy_pos_opt->x + hitbox_opt->offsetX, 
                        enemy_pos_opt->y + hitbox_opt->offsetY, 
                        hitbox_opt->width, 
                        hitbox_opt->height
                    };
                    Vector2 bulletPos = {bullet_pos_opt->x, bullet_pos_opt->y};
                    
                    if (CheckCollisionCircleRec(bulletPos, 20, enemyRect)) {
                        bullets_to_kill.push_back(reg.entity_from_index(bullet_idx));
                        
                        auto explosion = reg.spawn_entity();
                        reg.add_component(explosion, cpnt::Transform{bullet_pos_opt->x, bullet_pos_opt->y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                        reg.add_component(explosion, cpnt::Sprite{{114.0f, 18.0f, 17.0f, 16.0f}, 3.0f, 0, "bulletExplosion"});
                        reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                        reg.add_component(explosion, cpnt::Explosion{cpnt::Explosion::ExplosionType::Small, 0.0f, 0.08f, 0, 5});
                        //DrawCircleV(bulletPos, 20, RED); // Visual debug

                        auto& health = reg.get_components<cpnt::Health>()[enemy_idx];
                        if (health) {
                            health->hp--;
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
    for (auto [player_idx, player_pos_opt, player_tag_opt, player_hitbox_opt] : 
         ecs::indexed_zipper(positions, players, hitboxes)) {
        if (player_pos_opt && player_tag_opt && player_hitbox_opt) {
            for (auto [enemy_idx, enemy_pos_opt, enemy_tag_opt, health_opt, enemy_hitbox_opt] : 
                 ecs::indexed_zipper(positions, enemies, healths, hitboxes)) {
                if (enemy_pos_opt && enemy_tag_opt && health_opt && health_opt->hp > 0 && enemy_hitbox_opt) {
                    Rectangle enemyRect = {
                        enemy_pos_opt->x + enemy_hitbox_opt->offsetX, 
                        enemy_pos_opt->y + enemy_hitbox_opt->offsetY, 
                        enemy_hitbox_opt->width, 
                        enemy_hitbox_opt->height
                    };
                    Rectangle playerRect = {
                        player_pos_opt->x + player_hitbox_opt->offsetX, 
                        player_pos_opt->y + player_hitbox_opt->offsetY, 
                        player_hitbox_opt->width, 
                        player_hitbox_opt->height
                    };
                    
                    if (CheckCollisionRecs(playerRect, enemyRect)) {
                        // Handle player hit logic here
                        // For simplicity, just reduce enemy health
                        auto& health = reg.get_components<cpnt::Health>()[enemy_idx];
                        if (health) {
                            health->hp = 0;
                        }

                        auto& health_player = reg.get_components<cpnt::Health>()[player_idx];
                        if (health_player) {
                            health_player->hp -= 25; // Reduce player health
                        }
                    }
                }
            }
        }
    }
}
