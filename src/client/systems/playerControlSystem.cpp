#include "../components.h"
#include "../../ecs/registry.h"
#include "../../ecs/zipper.h"

#define WIDTH               1920
#define HEIGHT              1000

void PlayerControlSystem(ecs::Registry& reg,
                        ecs::SparseArray<Position> const& positions,
                        ecs::SparseArray<Player> const& players,
                        ecs::SparseArray<Sprite> const& sprites,
                        ecs::SparseArray<Velocity> const& velocities,
                        Texture2D& shipTexture,
                        Sound& shootSound,
                        float dt) {
    for (auto [idx, pos_opt, player_opt, sprite_opt, vel_opt] : ecs::indexed_zipper(positions, players, sprites, velocities)) {
        if (pos_opt && player_opt) {
            auto entity = reg.entity_from_index(idx);
            auto& pos = reg.get_components<Position>()[idx];
            auto& sprite = reg.get_components<Sprite>()[idx];
            auto& vel = reg.get_components<Velocity>()[idx];
            
            if (pos) {
                if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) pos->x += 400.0f * dt;
                if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) pos->x -= 400.0f * dt;
                if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) pos->y -= 400.0f * dt;
                if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) pos->y += 400.0f * dt;
                
                // Clamp position
                if (pos->x >= WIDTH - 99) pos->x = WIDTH - 99;
                if (pos->x <= 0) pos->x = 0;
                if (pos->y >= HEIGHT - 54) pos->y = HEIGHT - 54;
                if (pos->y <= 0) pos->y = 0;
                
                // Animate sprite
                if (sprite) {
                    if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
                        if (sprite->frame <= 5 && sprite->sourceRect.x != 232.0f) {
                            sprite->frame++;
                            sprite->sourceRect.x = 199.0f;
                        } else {
                            sprite->sourceRect.x = 232.0f;
                            sprite->frame = 0;
                        }
                    } else if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
                        if (sprite->frame <= 5 && sprite->sourceRect.x != 100.0f) {
                            sprite->frame++;
                            sprite->sourceRect.x = 133.0f;
                        } else {
                            sprite->sourceRect.x = 100.0f;
                            sprite->frame = 0;
                        }
                    } else {
                        if (sprite->frame <= 5 && (sprite->sourceRect.x == 100.0f || sprite->sourceRect.x == 133.0f)) {
                            sprite->frame ++;
                            sprite->sourceRect.x = 133.0f;
                        } else if (sprite->frame <= 5 && (sprite->sourceRect.x == 232.0f || sprite->sourceRect.x == 199.0f)) {
                            sprite->frame ++;
                            sprite->sourceRect.x = 199.0f;
                        } else {
                            sprite->sourceRect.x = 166.0f;
                            sprite->frame = 0;
                        }
                    }
                    if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
                        vel->z = -5.0f;
                    } else if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
                        vel->z = 5.0f;
                    } else {
                        vel->z = 0.0f;
                    }
                }
                
                // Shoot
                if (IsKeyPressed(KEY_SPACE)) {
                    auto bullet = reg.spawn_entity();
                    reg.add_component(bullet, Position{pos->x + 50, pos->y + 30});
                    reg.add_component(bullet, Velocity{15.0f, 0.0f});
                    reg.add_component(bullet, Bullet{});
                    reg.add_component(bullet, Sprite{{249.0f, 105.0f, 16.0f, 8.0f}, 2.0f});
                    PlaySound(shootSound);
                }
            }
        }
    }
}