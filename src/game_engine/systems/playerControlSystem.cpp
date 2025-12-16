#include "components/components.h"
#include "engine.h"
#include "ecs/zipper.h"
#include "raylib.h"

using namespace engn;

void PlayerControlSystem(EngineContext &ctx,
                        ecs::SparseArray<cpnt::Transform> const& positions,
                        ecs::SparseArray<cpnt::Player> const& players,
                        ecs::SparseArray<cpnt::Sprite> const& sprites,
                        ecs::SparseArray<cpnt::Velocity> const& velocities,
                        ecs::SparseArray<cpnt::Health> const& healths,
                        Texture2D& shipTexture,
                        Sound& shootSound)
{
    std::vector<ecs::Entity> to_remove;
    auto& reg = ctx.registry;
    float dt = ctx.delta_time;

    for (auto [idx, pos_opt, player_opt, vel_opt, hp_opt] : 
         ecs::indexed_zipper(positions, players, velocities, healths)) {
        // Handle player health
        auto& hp = reg.get_components<cpnt::Health>()[idx];
        auto& pos = reg.get_components<cpnt::Transform>()[idx];

        if (hp && hp->hp <= 0) {
            auto explosion = reg.spawn_entity();
            reg.add_component(explosion, cpnt::Transform{pos->x, pos->y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
            reg.add_component(explosion, cpnt::Sprite{{0.0f, 99.0f, 65.0f, 64.0f}, 2.0f, 0, "assets/sprites/r-typesheet44.gif"});
            reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
            reg.add_component(explosion, cpnt::Explosion{cpnt::Explosion::ExplosionType::Large, 0.0f, 0.08f, 0, 5});
            to_remove.push_back(reg.entity_from_index(idx));         
            continue;
        }
        
        if (pos_opt && player_opt) {
            auto entity = reg.entity_from_index(idx);
            auto& sprite = reg.get_components<cpnt::Sprite>()[idx];
            auto& vel = reg.get_components<cpnt::Velocity>()[idx];
            
            if (pos) {
                if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) pos->x += 400.0f * dt;
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) pos->x -= 400.0f * dt;
                if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) pos->y -= 400.0f * dt;
                if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) pos->y += 400.0f * dt;
                
                // Clamp position
                if (pos->x >= ctx.window_size.x - 99) pos->x = ctx.window_size.x - 99;
                if (pos->x <= 0) pos->x = 0;
                if (pos->y >= ctx.window_size.y - 54) pos->y = ctx.window_size.y - 54;
                if (pos->y <= 0) pos->y = 0;

                // Animate sprite
                if (sprite) {
                    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
                        if (sprite->frame <= 5 && sprite->source_rect.x != 232.0f) {
                            sprite->frame++;
                            sprite->source_rect.x = 199.0f;
                        } else {
                            sprite->source_rect.x = 232.0f;
                            sprite->frame = 0;
                        }
                    } else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
                        if (sprite->frame <= 5 && sprite->source_rect.x != 100.0f) {
                            sprite->frame++;
                            sprite->source_rect.x = 133.0f;
                        } else {
                            sprite->source_rect.x = 100.0f;
                            sprite->frame = 0;
                        }
                    } else {
                        if (sprite->frame <= 5 && (sprite->source_rect.x == 100.0f || sprite->source_rect.x == 133.0f)) {
                            sprite->frame ++;
                            sprite->source_rect.x = 133.0f;
                        } else if (sprite->frame <= 5 && (sprite->source_rect.x == 232.0f || sprite->source_rect.x == 199.0f)) {
                            sprite->frame ++;
                            sprite->source_rect.x = 199.0f;
                        } else {
                            sprite->source_rect.x = 166.0f;
                            sprite->frame = 0;
                        }
                    }
                    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                        vel->vz = -5.0f;
                    } else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                        vel->vz = 5.0f;
                    } else {
                        vel->vz = 0.0f;
                    }
                }
                
                // Rotation based on movement
                if (vel) {
                    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                        vel->vrz = -5.0f;
                    } else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                        vel->vrz = 5.0f;
                    } else {
                        vel->vrz = 0.0f;
                    }
                }
                
                // Shoot
                if (IsKeyPressed(KEY_SPACE)) {
                    auto bullet = reg.spawn_entity();
                    reg.add_component(bullet, cpnt::Transform{pos->x + 50, pos->y + 30, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                    reg.add_component(bullet, cpnt::Velocity{650.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                    reg.add_component(bullet, cpnt::Bullet{});
                    // reg.add_component(bullet, Sprite{{249.0f, 105.0f, 16.0f, 8.0f}, 2.0f, 0, shipTexture});
                    PlaySound(shootSound);
                }
            }
        }
    }

    for (auto e : to_remove) {
        reg.kill_entity(e);
    }
}
