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
} // namespace

void sys::enemy_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                       ecs::SparseArray<cpnt::Velocity> const& velocities, ecs::SparseArray<cpnt::Enemy> const& enemies,
                       ecs::SparseArray<cpnt::Health> const& healths, ecs::SparseArray<cpnt::Sprite> const& sprites) {
    auto& reg = ctx.registry;

    for (auto [idx, pos_opt, vel_opt, enemy_opt, health_opt] :
         ecs::indexed_zipper(positions, velocities, enemies, healths)) {
        if (pos_opt && vel_opt && enemy_opt && health_opt) {
            auto& pos = reg.get_components<cpnt::Transform>()[idx];
            auto& health = reg.get_components<cpnt::Health>()[idx];
            auto& sprite = reg.get_components<cpnt::Sprite>()[idx];
            auto& vel = reg.get_components<cpnt::Velocity>()[idx];

            if (pos && vel_opt) {
                // pos->x += vel_opt->vx;
                // pos->y += vel_opt->vy;
                // Respawn if off screen or dead
                // if (pos->x < k_offscreen_left || (health && health->hp <= 0)) {
                //     if (pos->x > k_offscreen_left) {
                //         auto explosion = reg.spawn_entity();
                //         reg.add_component(explosion,
                //                           cpnt::Transform{pos->x, pos->y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                //         reg.add_component(explosion, cpnt::Sprite{{0.0f, k_large_explosion_y, k_large_explosion_w,
                //                                                    k_large_explosion_h},
                //                                                   k_large_explosion_scale,
                //                                                   0,
                //                                                   "explosion"});
                //         reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                //         reg.add_component(explosion,
                //                           cpnt::Explosion{cpnt::Explosion::ExplosionType::Large, 0.0f,
                //                                           k_explosion_frame_duration, 0, k_explosion_frames});
                //     }

                //     // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
                //     pos->x =
                //         static_cast<float>(GetRandomValue(static_cast<int>(ctx.window_size.x),
                //                                           static_cast<int>(ctx.window_size.x * k_spawn_multiplier)));
                //     pos->y = static_cast<float>(
                //         GetRandomValue(k_spawn_margin, static_cast<int>(ctx.window_size.y) - k_spawn_margin));
                //     // NOLINTEND(cppcoreguidelines-pro-type-union-access)
                //     if (health) {
                //         health->hp = health->max_hp;
                //     }
                // }

                // Animate enemies

                if (vel_opt->vy > 0) {
                    if (sprite->frame <= k_animation_frame_limit && sprite->source_rect.x != k_sprite_up_right) {
                        sprite->frame++;
                        sprite->source_rect.x = k_sprite_up_anim;
                    } else {
                        sprite->source_rect.x = k_sprite_up_right;
                        sprite->frame = 0;
                    }
                } else if (vel_opt->vy < 0) {
                    if (sprite->frame <= k_animation_frame_limit && sprite->source_rect.x != k_sprite_down_left) {
                        sprite->frame++;
                        sprite->source_rect.x = k_sprite_down_anim;
                    } else {
                        sprite->source_rect.x = k_sprite_down_left;
                        sprite->frame = 0;
                    }
                } else {
                    if (sprite->frame <= k_animation_frame_limit &&
                        (sprite->source_rect.x == k_sprite_down_left || sprite->source_rect.x == k_sprite_down_anim)) {
                        sprite->frame++;
                        sprite->source_rect.x = k_sprite_down_anim;
                    } else if (sprite->frame <= k_animation_frame_limit &&
                               (sprite->source_rect.x == k_sprite_up_right ||
                                sprite->source_rect.x == k_sprite_up_anim)) {
                        sprite->frame++;
                        sprite->source_rect.x = k_sprite_up_anim;
                    } else {
                        sprite->source_rect.x = k_sprite_neutral;
                        sprite->frame = 0;
                    }
                }
            }
        }
    }
}
