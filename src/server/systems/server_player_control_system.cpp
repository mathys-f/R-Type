#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"

using namespace engn;

// Player movement
constexpr float k_player_speed = 400.0f;

// Player bounds
constexpr float k_player_width = 99.0f;
constexpr float k_player_height = 54.0f;

// Rotation
constexpr float k_rotation_speed = 5.0f;

// Bullet
constexpr float k_bullet_offset_x = 50.0f;
constexpr float k_bullet_offset_y = 30.0f;
constexpr float k_bullet_speed = 650.0f;
constexpr float k_bullet_width = 16.0f;
constexpr float k_bullet_height = 8.0f;
constexpr float k_bullet_scale = 2.0f;

void sys::server_player_control_system(EngineContext& ctx,
    ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::Player> const& players,
    ecs::SparseArray<cpnt::Velocity> const& velocities) {
    // LOG_DEBUG("Running server_player_control_system");
    auto& reg = ctx.registry;
    float dt = ctx.delta_time;

    for (auto [idx, pos_opt, player_opt, vel_opt] : ecs::indexed_zipper(positions, players, velocities)) {

        auto& pos = reg.get_components<cpnt::Transform>()[idx];

        if (pos_opt && player_opt) {
            auto& vel = reg.get_components<cpnt::Velocity>()[idx];

            if (pos) {
                const auto& input = ctx.input_state;
                pos->x += k_player_speed * dt * input.move_x;
                pos->y += k_player_speed * dt * input.move_y;
                ctx.registry.mark_dirty<cpnt::Transform>(ctx.registry.entity_from_index(idx));

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
                    reg.add_component(bullet, cpnt::Replicated{static_cast<std::uint32_t>(bullet)});
                }
            }
        }
    }
}
