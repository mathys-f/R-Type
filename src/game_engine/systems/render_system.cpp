#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"

using namespace engn;

namespace {
constexpr float k_particle_radius = 3.0f;
constexpr int k_alpha_max = 255;
    constexpr int k_level_to_appear = 0;
    constexpr int k_stars_cycle = 5;
} // namespace

void sys::render_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                        ecs::SparseArray<cpnt::Sprite> const& sprites, ecs::SparseArray<cpnt::Star> const& stars,
                        ecs::SparseArray<cpnt::Velocity> const& velocities, ecs::SparseArray<cpnt::Particle> const& particles,
                        ecs::SparseArray<cpnt::Stats> const& stats, ecs::SparseArray<cpnt::Boss> const& bosses) {

    auto& reg = ctx.registry;
    Color stars_color = WHITE;

    // Get Stats
    for (auto [stats_idx, stats_opt] : ecs::indexed_zipper(stats)) {
        if (stats_opt) {
            auto& stat = reg.get_components<cpnt::Stats>()[stats_idx];
            // Change star color based on level
            switch (stat->level % k_stars_cycle) {
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

    //  Render boss wave effect
    for (auto [boss_idx, boss_opt] : ecs::indexed_zipper(bosses)) {
        if (boss_opt && boss_opt->roar_active) {
            float radius = boss_opt->waveRadius;
            int center_x = (int)boss_opt->waveCenter.x;
            int center_y = (int)boss_opt->waveCenter.y;
            
            // Draw multiple trailing rings with decreasing thickness and alpha
            const int k_num_rings = 8;
            const float k_ring_spacing = 15.0f;
            
            for (int i = k_num_rings - 1; i >= 0; i--) {
                float ringRadius = radius - (i * k_ring_spacing);  // NOLINT(cppcoreguidelines-narrowing-conversions,-warnings-as-errors)
                
                if (ringRadius > 0) {
                    // Calculate alpha fade (older rings are more transparent)
                    float alpha = 1.0f - (i / (float)k_num_rings); // NOLINT(cppcoreguidelines-narrowing-conversions,-warnings-as-errors)
                    alpha = alpha * alpha; // Quadratic falloff for smoother fade
                    
                    // Calculate thickness (thicker at the front)
                    int thickness = (i == 0) ? 10 : (i <= 2) ? 3 : 2; // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                    
                    Color ring_color = Fade(WHITE, alpha);
                    
                    // Draw multiple lines for thickness
                    for (int t = 0; t < thickness; t++) {
                        DrawCircleLines(center_x, center_y, ringRadius + t, ring_color); // NOLINT(cppcoreguidelines-narrowing-conversions,-warnings-as-errors)
                    }
                }
            }
        }
    }
}
