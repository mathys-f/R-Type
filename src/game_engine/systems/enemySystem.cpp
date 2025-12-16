#include "components/components.h"
#include "engine.h"
#include "ecs/zipper.h"
#include "raylib.h"
#include "systems/systems.h"

using namespace engn;

void sys::EnemySystem(EngineContext &ctx,
                ecs::SparseArray<cpnt::Transform> const& positions,
                ecs::SparseArray<cpnt::Velocity> const& velocities,
                ecs::SparseArray<cpnt::Enemy> const& enemies,
                ecs::SparseArray<cpnt::Health> const& healths,
                ecs::SparseArray<cpnt::Sprite> const& sprites)
{
    auto& reg = ctx.registry;
    
    for (auto [idx, pos_opt, vel_opt, enemy_opt, health_opt] : 
         ecs::indexed_zipper(positions, velocities, enemies, healths)) {
        if (pos_opt && vel_opt && enemy_opt && health_opt) {
            auto& pos = reg.get_components<cpnt::Transform>()[idx];
            auto& health = reg.get_components<cpnt::Health>()[idx];
            auto& sprite = reg.get_components<cpnt::Sprite>()[idx];
            auto& vel = reg.get_components<cpnt::Velocity>()[idx];
            
            if (pos && vel_opt) {
                pos->x += vel_opt->vx;
                pos->y += vel_opt->vy;
                // Respawn if off screen or dead
                if (pos->x < -100 || (health && health->hp <= 0)) {
                    if (pos->x > -100) {
                        auto explosion = reg.spawn_entity();
                        reg.add_component(explosion, cpnt::Transform{pos->x, pos->y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                        reg.add_component(explosion, cpnt::Sprite{{0.0f, 99.0f, 65.0f, 64.0f}, 2.0f, 0, "assets/sprites/r-typesheet44.gif"});
                        reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                        reg.add_component(explosion, cpnt::Explosion{cpnt::Explosion::ExplosionType::Large, 0.0f, 0.08f, 0, 5});
                    }
                    
                    pos->x = GetRandomValue(ctx.window_size.x, ctx.window_size.x * 1.5);
                    pos->y = GetRandomValue(100, ctx.window_size.y - 100);
                    if (health) {
                        health->hp = health->max_hp;
                    }
                }

                //Animate enemies

                if (vel_opt->vy > 0) {
                    if (sprite->frame <= 15 && sprite->source_rect.x != 137.0f) {
                        sprite->frame++;
                        sprite->source_rect.x = 104.0f;
                    } else {
                        sprite->source_rect.x = 137.0f;
                        sprite->frame = 0;
                    }
                } else if (vel_opt->vy < 0) {
                    if (sprite->frame <= 15 && sprite->source_rect.x != 5.0f) {
                        sprite->frame++;
                        sprite->source_rect.x = 38.0f;
                    } else {
                        sprite->source_rect.x = 5.0f;
                        sprite->frame = 0;
                    }
                } else {
                    if (sprite->frame <= 15 && (sprite->source_rect.x == 5.0f || sprite->source_rect.x == 38.0f)) {
                        sprite->frame ++;
                        sprite->source_rect.x = 38.0f;
                    } else if (sprite->frame <= 15 && (sprite->source_rect.x == 137.0f || sprite->source_rect.x == 104.0f)) {
                        sprite->frame ++;
                        sprite->source_rect.x = 104.0f;
                    } else {
                        sprite->source_rect.x = 71.0f;
                        sprite->frame = 0;
                    }
                }
            }
        }
    }
}
