#include "../components.h"
#include "../../ecs/registry.h"
#include "../../ecs/zipper.h"

void EnemySystem(ecs::Registry& reg,
                ecs::SparseArray<Position> const& positions,
                ecs::SparseArray<Velocity> const& velocities,
                ecs::SparseArray<Enemy> const& enemies,
                ecs::SparseArray<Health> const& healths,
                ecs::SparseArray<Sprite> const& sprites) {
    for (auto [idx, pos_opt, vel_opt, enemy_opt, health_opt, spr_opt] : 
         ecs::indexed_zipper(positions, velocities, enemies, healths, sprites)) {
        if (pos_opt && vel_opt && enemy_opt && health_opt) {
            auto& pos = reg.get_components<Position>()[idx];
            auto& health = reg.get_components<Health>()[idx];
            auto& sprite = reg.get_components<Sprite>()[idx];
            auto& vel = reg.get_components<Velocity>()[idx];
            
            if (pos && vel_opt) {
                pos->x += vel_opt->x;
                pos->y += vel_opt->y;
                // Respawn if off screen or dead
                if (pos->x < -100 || (health && health->current <= 0)) {
                    pos->x = GetRandomValue(WIDTH, WIDTH * 1.5);
                    pos->y = GetRandomValue(100, HEIGHT -100);
                    if (health) {
                        health->current = health->max;
                    }
                }
                
                //Animate enemies

                if (vel_opt->y > 0) {
                    if (sprite->frame <= 15 && sprite->sourceRect.x != 137.0f) {
                        sprite->frame++;
                        sprite->sourceRect.x = 104.0f;
                    } else {
                        sprite->sourceRect.x = 137.0f;
                        sprite->frame = 0;
                    }
                } else if (vel_opt->y < 0) {
                    if (sprite->frame <= 15 && sprite->sourceRect.x != 5.0f) {
                        sprite->frame++;
                        sprite->sourceRect.x = 38.0f;
                    } else {
                        sprite->sourceRect.x = 5.0f;
                        sprite->frame = 0;
                    }
                } else {
                    if (sprite->frame <= 15 && (sprite->sourceRect.x == 5.0f || sprite->sourceRect.x == 38.0f)) {
                        sprite->frame ++;
                        sprite->sourceRect.x = 38.0f;
                    } else if (sprite->frame <= 15 && (sprite->sourceRect.x == 137.0f || sprite->sourceRect.x == 104.0f)) {
                        sprite->frame ++;
                        sprite->sourceRect.x = 104.0f;
                    } else {
                        sprite->sourceRect.x = 71.0f;
                        sprite->frame = 0;
                    }
                }

            }
        }
    }
}