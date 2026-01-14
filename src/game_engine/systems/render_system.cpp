#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"

using namespace engn;

namespace {
constexpr float k_particle_radius = 3.0f;
constexpr int k_alpha_max = 255;
} // namespace

void sys::render_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                        ecs::SparseArray<cpnt::Sprite> const& sprites, ecs::SparseArray<cpnt::Star> const& stars,
                        ecs::SparseArray<cpnt::Velocity> const& velocities,
                        ecs::SparseArray<cpnt::Particle> const& particles, ecs::SparseArray<cpnt::Stats> const& stats) {

    auto& reg = ctx.registry;
    Color stars_color = WHITE;


    // Get Stats
    for (auto [stats_idx, stats_opt] : ecs::indexed_zipper(stats)) {
        if (stats_opt) {
            auto& stat = reg.get_components<cpnt::Stats>()[stats_idx];
            // Change star color based on level
            switch (stat->level % 5) {
                case 0:
                    stars_color = WHITE;
                    break;
                case 1:
                    stars_color = LIME;
                    break;
                case 2:
                    stars_color = SKYBLUE;
                    break;
                case 3:
                    stars_color = PINK;
                    break;
                case 4:
                    stars_color = GOLD;
                    break;
                default:
                    stars_color = WHITE;
                    break;
            }
        }
    }

    // Render stars
    for (auto [idx, pos_opt, star_opt] : ecs::indexed_zipper(positions, stars)) {
        if (pos_opt && star_opt) {
            DrawPixel(static_cast<int>(pos_opt->x), static_cast<int>(pos_opt->y), stars_color);
        }
    }

    // Render particles
    for (auto [idx, pos_opt, particle_opt] : ecs::indexed_zipper(positions, particles)) {
        if (pos_opt && particle_opt) {
            float alpha = 1.0f - (particle_opt->lifetime / particle_opt->max_lifetime);
            DrawCircleV(
                (Vector2){pos_opt->x, pos_opt->y}, k_particle_radius * alpha,
                (Color){particle_opt->red, particle_opt->green, particle_opt->blue, static_cast<unsigned char>(alpha * k_alpha_max)});

            // Update lifetime
            auto& particles_arr = reg.get_components<cpnt::Particle>();
            if (idx < particles_arr.size() && particles_arr[idx]) {
                auto& particle = *particles_arr[idx];
                particle.lifetime += GetFrameTime();
                if (particle.lifetime >= particle.max_lifetime) {
                    auto entity = reg.entity_from_index(idx);
                    reg.kill_entity(entity);
                }
            }
        }
    }

    // Render everything with sprites
    for (auto [idx, pos_opt, sprite_opt, vel_opt] : ecs::indexed_zipper(positions, sprites, velocities)) {
        if (pos_opt && sprite_opt) {
            float width = sprite_opt->source_rect.width * sprite_opt->scale;
            float height = sprite_opt->source_rect.height * sprite_opt->scale;

            std::optional<Texture2D> texture = ctx.assets_manager.get_asset<Texture2D>(sprite_opt->texture);
            if (texture.has_value()) {
                DrawTexturePro(texture.value(), sprite_opt->source_rect,
                               (Rectangle){pos_opt->x, pos_opt->y, width, height},
                               (Vector2){pos_opt->origin_x, pos_opt->origin_y}, vel_opt ? vel_opt->vz : 0.0f, WHITE);
            }
        }
    }
}
