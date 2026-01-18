#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"

using namespace engn;

namespace {
// Explosion constants
constexpr float k_explosion_sprite_y = 99.0f;
constexpr float k_explosion_sprite_w = 65.0f;
constexpr float k_explosion_sprite_h = 64.0f;
constexpr float k_explosion_scale = 2.0f;
constexpr float k_explosion_frame_duration = 0.08f;
constexpr int k_explosion_frames = 5;

// Player movement
constexpr float k_player_speed = 400.0f;

// Player bounds
constexpr float k_player_width = 99.0f;
constexpr float k_player_height = 54.0f;

// Animation
constexpr int k_animation_frame_limit = 5;
constexpr float k_sprite_down_left = 100.0f;
constexpr float k_sprite_down_anim = 133.0f;
constexpr float k_sprite_neutral = 166.0f;
constexpr float k_sprite_up_anim = 199.0f;
constexpr float k_sprite_up_right = 232.0f;

// Rotation
constexpr float k_rotation_speed = 5.0f;

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

void sys::player_control_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                                ecs::SparseArray<cpnt::Player> const& players,
                                ecs::SparseArray<cpnt::Sprite> const& sprites,
                                ecs::SparseArray<cpnt::Velocity> const& velocities,
                                ecs::SparseArray<cpnt::Health> const& healths) {
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
            reg.add_component(explosion,
                              cpnt::Sprite{{0.0f, k_explosion_sprite_y, k_explosion_sprite_w, k_explosion_sprite_h},
                                           k_explosion_scale,
                                           0,
                                           "explosion"});
            reg.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
            reg.add_component(explosion, cpnt::Explosion{cpnt::Explosion::ExplosionType::Large, 0.0f,
                                                         k_explosion_frame_duration, 0, k_explosion_frames});
            to_remove.push_back(reg.entity_from_index(idx));
            continue;
        }

        if (pos_opt && player_opt) {
            auto& sprite = reg.get_components<cpnt::Sprite>()[idx];
            auto& vel = reg.get_components<cpnt::Velocity>()[idx];

            if (pos) {
                const auto& input = ctx.input_state;
                pos->x += k_player_speed * dt * input.move_x;
                pos->y += k_player_speed * dt * input.move_y;

                // Clamp position
                // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
                if (pos->x >= ctx.window_size.x - k_player_width)
                    pos->x = ctx.window_size.x - k_player_width;
                if (pos->x <= 0)
                    pos->x = 0;
                if (pos->y >= ctx.window_size.y - k_player_height)
                    pos->y = ctx.window_size.y - k_player_height;
                if (pos->y <= 0)
                    pos->y = 0;
                // NOLINTEND(cppcoreguidelines-pro-type-union-access)

                // Animate sprite
                if (sprite) {
                    if (input.move_y < 0.0f) {
                        if (sprite->frame <= k_animation_frame_limit && sprite->source_rect.x != k_sprite_up_right) {
                            sprite->frame++;
                            sprite->source_rect.x = k_sprite_up_anim;
                        } else {
                            sprite->source_rect.x = k_sprite_up_right;
                            sprite->frame = 0;
                        }
                    } else if (input.move_y > 0.0f) {
                        if (sprite->frame <= k_animation_frame_limit && sprite->source_rect.x != k_sprite_down_left) {
                            sprite->frame++;
                            sprite->source_rect.x = k_sprite_down_anim;
                        } else {
                            sprite->source_rect.x = k_sprite_down_left;
                            sprite->frame = 0;
                        }
                    } else {
                        if (sprite->frame <= k_animation_frame_limit && (sprite->source_rect.x == k_sprite_down_left ||
                                                                         sprite->source_rect.x == k_sprite_down_anim)) {
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
                    if (input.move_x < 0.0f) {
                        vel->vz = -k_rotation_speed;
                    } else if (input.move_x > 0.0f) {
                        vel->vz = k_rotation_speed;
                    } else {
                        vel->vz = 0.0f;
                    }
                }

                // Rotation based on movement
                if (vel) {
                    if (input.move_x < 0.0f) {
                        vel->vrz = -k_rotation_speed;
                    } else if (input.move_x > 0.0f) {
                        vel->vrz = k_rotation_speed;
                    } else {
                        vel->vrz = 0.0f;
                    }
                }

                // Shoot
                if (input.shoot_pressed) {
                    auto bullet = reg.spawn_entity();
                    reg.add_component(bullet, cpnt::Transform{pos->x + k_bullet_offset_x, pos->y + k_bullet_offset_y,
                                                              0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                    reg.add_component(bullet, cpnt::Velocity{k_bullet_speed, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                    reg.add_component(bullet, cpnt::Bullet{});
                    reg.add_component(bullet, cpnt::Hitbox{20.0f, 20.0f, k_bullet_width, k_bullet_height}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                    reg.add_component(
                        bullet, cpnt::Sprite{{k_bullet_sprite_x, k_bullet_sprite_y, k_bullet_width, k_bullet_height},
                                             k_bullet_scale,
                                             0,
                                             "player_ship"});
                    std::optional<Sound> shoot_sound = ctx.assets_manager.get_asset<Sound>("shoot_sound");
                    if (shoot_sound.has_value())
                        PlaySound(shoot_sound.value());
                }
            }
        }
    }

    for (auto e : to_remove) {
        reg.kill_entity(e);
    }
}
