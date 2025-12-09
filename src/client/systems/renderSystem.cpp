#include "../components.h"
#include "../../ecs/registry.h"
#include "../../ecs/zipper.h"

void RenderSystem(ecs::Registry& reg, 
                 ecs::SparseArray<Position> const& positions,
                 ecs::SparseArray<Sprite> const& sprites,
                 ecs::SparseArray<Star> const& stars,
                 ecs::SparseArray<Velocity> const& velocities,
                 ecs::SparseArray<Particle> const& particles,
                 Texture2D& shipTexture,
                 Texture2D& enemyTexture) {
    
    // Render stars
    for (auto [idx, pos_opt, star_opt] : ecs::indexed_zipper(positions, stars)) {
        if (pos_opt && star_opt) {
            DrawPixel(pos_opt->x, pos_opt->y, WHITE);
        }
    }

    // Render particles
    for (auto [idx, pos_opt, particle_opt] : ecs::indexed_zipper(positions, particles)) {
        if (pos_opt && particle_opt) {
            float alpha = 1.0f - (particle_opt->lifetime / particle_opt->maxLifetime);
            DrawCircleV((Vector2){pos_opt->x, pos_opt->y}, 3.0f * alpha, (Color){0, 165, 255, (unsigned char)(alpha * 255)});
            
            // Update lifetime
            auto& particle = reg.get_components<Particle>()[idx];
            if (particle) {
                particle->lifetime += GetFrameTime();
                if (particle->lifetime >= particle->maxLifetime) {
                    auto entity = reg.entity_from_index(idx);
                    reg.kill_entity(entity);
                }
            }
        }
    }
    
    // Render everything with sprites
    for (auto [idx, pos_opt, sprite_opt, vel_opt] : ecs::indexed_zipper(positions, sprites, velocities)) {
        if (pos_opt && sprite_opt) {
            auto& enemies_arr = reg.get_components<Enemy>();
            bool is_enemy = idx < enemies_arr.size() && enemies_arr[idx].has_value();
            
            Texture2D* texture = is_enemy ? &enemyTexture : &shipTexture;
            
            float width = sprite_opt->sourceRect.width * sprite_opt->scale;
            float height = sprite_opt->sourceRect.height * sprite_opt->scale;
            
            DrawTexturePro(*texture, 
                          sprite_opt->sourceRect,
                          (Rectangle){pos_opt->x + sprite_opt->sourceRect.width, pos_opt->y + sprite_opt->sourceRect.height, width, height},
                          (Vector2){pos_opt->origin.x, pos_opt->origin.y}, 
                          vel_opt ? vel_opt->z : 0.0f,
                          WHITE);
        }
    }
}