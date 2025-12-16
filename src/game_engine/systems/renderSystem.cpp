#include "components/components.h"
#include "engine.h"
#include "ecs/zipper.h"
#include "systems/systems.h"
#include "raylib.h"

using namespace engn;

void sys::RenderSystem(EngineContext &ctx,
                 ecs::SparseArray<cpnt::Transform> const& positions,
                 ecs::SparseArray<cpnt::Sprite> const& sprites,
                 ecs::SparseArray<cpnt::Star> const& stars,
                 ecs::SparseArray<cpnt::Velocity> const& velocities,
                 ecs::SparseArray<cpnt::Particle> const& particles)
{
    
    auto& reg = ctx.registry;
    // Render stars
    for (auto [idx, pos_opt, star_opt] : ecs::indexed_zipper(positions, stars)) {
        if (pos_opt && star_opt) {
            DrawPixel(static_cast<int>(pos_opt->x), static_cast<int>(pos_opt->y), WHITE);
        }
    }

    // Render particles
    for (auto [idx, pos_opt, particle_opt] : ecs::indexed_zipper(positions, particles)) {
        if (pos_opt && particle_opt) {
            float alpha = 1.0f - (particle_opt->lifetime / particle_opt->maxLifetime);
            DrawCircleV((Vector2){pos_opt->x, pos_opt->y}, 3.0f * alpha, 
                       (Color){0, 165, 255, (unsigned char)(alpha * 255)});
            
            // Update lifetime
            auto& particles_arr = reg.get_components<cpnt::Particle>();
            if (idx < particles_arr.size() && particles_arr[idx]) {
                auto& particle = *particles_arr[idx];
                particle.lifetime += GetFrameTime();
                if (particle.lifetime >= particle.maxLifetime) {
                    auto entity = reg.entity_from_index(idx);
                    reg.kill_entity(entity);
                }
            }
        }
    }
    
    // Render everything with sprites
    for (auto [idx, pos_opt, sprite_opt, vel_opt] : ecs::indexed_zipper(positions, sprites, velocities)) {
        if (pos_opt && sprite_opt) {
            auto& enemies_arr = reg.get_components<cpnt::Enemy>();
            bool is_enemy = idx < enemies_arr.size() && enemies_arr[idx].has_value();

            float width = sprite_opt->source_rect.width * sprite_opt->scale;
            float height = sprite_opt->source_rect.height * sprite_opt->scale;
        }
    }
}
