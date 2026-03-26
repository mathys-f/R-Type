#include "ecs/zipper.h"
#include "game_engine/components/components.h"
#include "game_engine/engine.h"
#include "systems/systems.h"

using namespace engn;

namespace {
// Player movement
constexpr float k_player_speed = 400.0f;

// Player bounds
constexpr float k_player_width = 99.0f;
constexpr float k_player_height = 33.0f;

// Animation
constexpr int k_animation_frame_limit = 5;
constexpr float k_sprite_down_left = 1.0f;
constexpr float k_sprite_down_anim = 34.0f;
constexpr float k_sprite_neutral = 67.0f;
constexpr float k_sprite_up_anim = 100.0f;
constexpr float k_sprite_up_right = 133.0f;
constexpr float k_sprite_y_add = 17.0f;

// Rotation
constexpr float k_rotation_speed = 5.0f;
} // namespace

void sys::predict_local_player_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                                      ecs::SparseArray<cpnt::Player> const& players,
                                      ecs::SparseArray<cpnt::Sprite> const& sprites,
                                      ecs::SparseArray<cpnt::Velocity> const& velocities) {
    if (!ctx.network_client || !ctx.network_client->is_connected()) {
        return;
    }

    const std::uint8_t k_local_player_id = static_cast<std::uint8_t>(ctx.network_client->player_id());
    auto& reg = ctx.registry;
    float dt = ctx.delta_time;

    for (auto [idx, pos_opt, player_opt, sprite_opt, vel_opt] :
         ecs::indexed_zipper(positions, players, sprites, velocities)) {
        if (!pos_opt || !player_opt) {
            continue;
        }
        if (player_opt->id != k_local_player_id) {
            continue;
        }

        auto& pos = reg.get_components<cpnt::Transform>()[idx];
        auto& sprite = reg.get_components<cpnt::Sprite>()[idx];
        auto& vel = reg.get_components<cpnt::Velocity>()[idx];
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

        if (sprite) {
            if (input.move_y < 0.0f) {
                if (sprite->frame <= k_animation_frame_limit && sprite->source_rect.x != k_sprite_up_right) {
                    sprite->frame++;
                    sprite->source_rect.x = k_sprite_up_anim;
                    sprite->source_rect.y = k_sprite_y_add * static_cast<float>(k_local_player_id);
                } else {
                    sprite->source_rect.x = k_sprite_up_right;
                    sprite->source_rect.y = k_sprite_y_add * static_cast<float>(k_local_player_id);
                    sprite->frame = 0;
                }
            } else if (input.move_y > 0.0f) {
                if (sprite->frame <= k_animation_frame_limit && sprite->source_rect.x != k_sprite_down_left) {
                    sprite->frame++;
                    sprite->source_rect.x = k_sprite_down_anim;
                    sprite->source_rect.y = k_sprite_y_add * static_cast<float>(k_local_player_id);
                } else {
                    sprite->source_rect.x = k_sprite_down_left;
                    sprite->source_rect.y = k_sprite_y_add * static_cast<float>(k_local_player_id);
                    sprite->frame = 0;
                }
            } else {
                if (sprite->frame <= k_animation_frame_limit &&
                    (sprite->source_rect.x == k_sprite_down_left || sprite->source_rect.x == k_sprite_down_anim)) {
                    sprite->frame++;
                    sprite->source_rect.x = k_sprite_down_anim;
                    sprite->source_rect.y = k_sprite_y_add * static_cast<float>(k_local_player_id);
                } else if (sprite->frame <= k_animation_frame_limit &&
                           (sprite->source_rect.x == k_sprite_up_right || sprite->source_rect.x == k_sprite_up_anim)) {
                    sprite->frame++;
                    sprite->source_rect.x = k_sprite_up_anim;
                    sprite->source_rect.y = k_sprite_y_add * static_cast<float>(k_local_player_id);
                } else {
                    sprite->source_rect.x = k_sprite_neutral;
                    sprite->source_rect.y = k_sprite_y_add * static_cast<float>(k_local_player_id);
                    sprite->frame = 0;
                }
            }
            if (vel) {
                if (input.move_x < 0.0f) {
                    vel->vz = -k_rotation_speed;
                } else if (input.move_x > 0.0f) {
                    vel->vz = k_rotation_speed;
                } else {
                    vel->vz = 0.0f;
                }
            }
        }

        if (vel) {
            if (input.move_x < 0.0f) {
                vel->vrz = -k_rotation_speed;
            } else if (input.move_x > 0.0f) {
                vel->vrz = k_rotation_speed;
            } else {
                vel->vrz = 0.0f;
            }
        }
    }
}
