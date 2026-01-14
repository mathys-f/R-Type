#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"

using namespace engn;

namespace {
constexpr float k_offscreen_left = -100.0f;
constexpr int k_spawn_margin = 100;
constexpr float k_spawn_multiplier = 1.5f;
constexpr float k_large_explosion_y = 99.0f;
constexpr float k_large_explosion_w = 65.0f;
constexpr float k_large_explosion_h = 64.0f;
constexpr float k_large_explosion_scale = 2.0f;
constexpr float k_explosion_frame_duration = 0.08f;
constexpr int k_explosion_frames = 5;
constexpr int k_animation_frame_limit = 15;
constexpr float k_sprite_down_left = 5.0f;
constexpr float k_sprite_down_anim = 38.0f;
constexpr float k_sprite_neutral = 71.0f;
constexpr float k_sprite_up_anim = 104.0f;
constexpr float k_sprite_up_right = 137.0f;

// Bullet
constexpr float k_bullet_offset_x = 50.0f;
constexpr float k_bullet_offset_y = 30.0f;
constexpr float k_bullet_speed = 650.0f;
constexpr float k_bullet_width = 16.0f;
constexpr float k_bullet_height = 8.0f;
constexpr float k_bullet_sprite_x = 249.0f;
constexpr float k_bullet_sprite_y = 105.0f;
constexpr float k_bullet_scale = 2.0f;
} // namespace

void sys::shooter_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                       ecs::SparseArray<cpnt::Velocity> const& velocities,
                       ecs::SparseArray<cpnt::Health> const& healths, ecs::SparseArray<cpnt::Sprite> const& sprites,
                       ecs::SparseArray<cpnt::Shooter> const& shooters, ecs::SparseArray<cpnt::Player> const& player) {
    auto& reg = ctx.registry;
    float dt = ctx.delta_time;

    for (auto [idx, pos_opt, vel_opt, shot_opt, health_opt] :
         ecs::indexed_zipper(positions, velocities, shooters, healths)) {
        if (pos_opt && vel_opt && shot_opt && health_opt) {
            auto& pos = reg.get_components<cpnt::Transform>()[idx];
            auto& health = reg.get_components<cpnt::Health>()[idx];
            auto& sprite = reg.get_components<cpnt::Sprite>()[idx];
            auto& vel = reg.get_components<cpnt::Velocity>()[idx];
            auto& shot = reg.get_components<cpnt::Shooter>()[idx];

            if (pos && vel_opt) {
                pos->x += vel_opt->vx;
                pos->y += vel_opt->vy;
                // Respawn if off screen or dead
                if (pos->x < k_offscreen_left || (health && health->hp <= 0)) {
                    if (pos->x > k_offscreen_left) {
                        auto explosion = reg.spawn_entity();
                        reg.add_component(explosion,
                                          cpnt::Transform{pos->x, pos->y, 0.0f, 55.f, 45.f, 0.0f, 1.0f, 1.0f, 1.0f}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                        reg.add_component(explosion, cpnt::Sprite{{0.0f, k_large_explosion_y, k_large_explosion_w,
                                                                   k_large_explosion_h},
                                                                  k_large_explosion_scale,
                                                                  0,
                                                                  "explosion"});
                        reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                        reg.add_component(explosion,
                                          cpnt::Explosion{cpnt::Explosion::ExplosionType::Large, 0.0f,
                                                          k_explosion_frame_duration, 0, k_explosion_frames});
                    }

                    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
                    pos->x =
                        static_cast<float>(GetRandomValue(static_cast<int>(ctx.k_window_size.x),
                                                          static_cast<int>(ctx.k_window_size.x * k_spawn_multiplier)));
                    pos->y = static_cast<float>(
                        GetRandomValue(k_spawn_margin, static_cast<int>(ctx.k_window_size.y) - k_spawn_margin));
                    // NOLINTEND(cppcoreguidelines-pro-type-union-access)
                    if (health) {
                        health->hp = health->max_hp;
                    }
                }
            }

            // Shoot projectiles at player
            shot->timer += dt;
            if (shot->timer >= 1.0f && pos_opt->x < ctx.k_window_size.x) { // NOLINT(cppcoreguidelines-pro-type-union-access,-warnings-as-errors)
                float dx = 0.0f;
                float dy = 0.0f;
                for (auto [pidx, ppos_opt, pplay_opt] : ecs::indexed_zipper(positions, player)) {
                    if (!ppos_opt || !pplay_opt)
                        continue;
                    auto& ppos = reg.get_components<cpnt::Transform>()[pidx];
                    dx = ppos->x - pos->x;
                    dy = ppos->y - pos->y;
                    break; // Assuming only one player
                }
                float length = sqrt(dx * dx + dy * dy); // NOLINT(cppcoreguidelines-narrowing-conversions,-warnings-as-errors)
                // NOLINTBEGIN(readability-identifier-naming,-warnings-as-errors)
                float dirX = dx / length;
                float dirY = dy / length;
                const float speed = 400.0f;
                float vx = dirX * speed;
                float vy = dirY * speed;
                // NOLINTEND(readability-identifier-naming,-warnings-as-errors)
                auto bullet = reg.spawn_entity();
                reg.add_component(bullet, cpnt::Transform{pos->x, pos->y, 0.0f, 16.f/2, 8.0f/2, 0.0f, 1.0f, 1.0f, 1.0f}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                reg.add_component(bullet, cpnt::Velocity{vx, vy, vel->vz + 180.0f, 0.0f, 0.0f, 0.0f});  // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                reg.add_component(bullet, cpnt::Bullet_shooter{});
                reg.add_component(bullet, cpnt::Hitbox{16.0f, 8.0f, 0.f, 0.f}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                reg.add_component(
                    bullet, cpnt::Sprite{{k_bullet_sprite_x, k_bullet_sprite_y, k_bullet_width, k_bullet_height},
                                 k_bullet_scale,
                                 0,
                                 "shooter_bullet"});
                std::optional<Sound> shoot_sound = ctx.assets_manager.get_asset<Sound>("shoot_sound");
                if (shoot_sound.has_value())
                            PlaySound(shoot_sound.value());
                shot->timer = 0.0f;
            }
        }
    }
}
