#include "components/components.h"
#include "ecs/zipper.h"
#include "engine.h"
#include "raylib.h"
#include "systems/systems.h"

#include <mutex>

using namespace engn;

// Player movement
constexpr float k_player_speed = 400.0f;

// Player bounds
constexpr float k_player_width = 99.0f;
constexpr float k_player_height = 33.0f;

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
            auto& player = reg.get_components<cpnt::Player>()[idx];

            if (pos && player) {
                float move_x = 0.0f;
                float move_y = 0.0f;
                bool shoot_pressed = false;

                {
                    std::lock_guard<std::mutex> lock(ctx.player_input_queues_mutex);
                    // Convert player ID to endpoint (guarded against network thread updates)
                    auto endpoint_it = ctx.player_id_to_endpoint.find(player->id);
                    if (endpoint_it == ctx.player_id_to_endpoint.end()) {
                        continue; // Player ID not mapped to endpoint
                    }

                    auto queue_it = ctx.player_input_queues.find(endpoint_it->second);
                    if (queue_it == ctx.player_input_queues.end()) {
                        continue; // No input for this player
                    }

                    auto &input_queue = queue_it->second;

                    input_queue.for_each<evts::KeyHold>([&](const evts::KeyHold& key_hold) {
                        if (key_hold.keycode == ctx.controls.move_up.primary) {
                            move_y = -1.0f;
                        }
                        if (key_hold.keycode == ctx.controls.move_down.primary) {
                            move_y = 1.0f;
                        }
                        if (key_hold.keycode == ctx.controls.move_left.primary) {
                            move_x = -1.0f;
                        }
                        if (key_hold.keycode == ctx.controls.move_right.primary) {
                            move_x = 1.0f;
                        }
                    });

                    input_queue.for_each<evts::KeyPressed>([&](const evts::KeyPressed& key_pressed) {
                        if (key_pressed.keycode == ctx.controls.shoot.primary) {
                            shoot_pressed = true;
                        }
                    });

                    input_queue.clear();
                }

                pos->x += k_player_speed * dt * move_x;
                pos->y += k_player_speed * dt * move_y;
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
                    ctx.registry.mark_dirty<cpnt::Velocity>(ctx.registry.entity_from_index(idx));
                    if (move_x < 0.0f) {
                        vel->vz = -k_rotation_speed;
                        vel->vrz = -k_rotation_speed;
                    } else if (move_x > 0.0f) {
                        vel->vz = k_rotation_speed;
                        vel->vrz = k_rotation_speed;
                    } else {
                        vel->vz = 0.0f;
                        vel->vrz = 0.0f;
                    }
                }

                // Shoot
                if (shoot_pressed) {
                    // Calculate bullet spawn position
                    float bullet_x = pos->x + k_bullet_offset_x;
                    float bullet_y = pos->y + k_bullet_offset_y;

                    auto bullet = reg.spawn_entity();
                    // Add Replicated and EntityType FIRST to ensure they're in the snapshot
                    reg.add_component(bullet, cpnt::Replicated{static_cast<std::uint32_t>(bullet)});
                    reg.add_component(bullet, cpnt::EntityType{"bullet"});
                    // Then add Transform and other critical components
                    reg.add_component(bullet, cpnt::Transform{bullet_x, bullet_y,
                                                              0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                    reg.add_component(bullet, cpnt::Velocity{k_bullet_speed, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                    reg.add_component(bullet, cpnt::Bullet{});
                    reg.add_component(bullet, cpnt::Hitbox{20.0f, 20.0f, k_bullet_width, k_bullet_height}); // NOLINT(cppcoreguidelines-avoid-magic-numbers,-warnings-as-errors)
                }
            }
        }
    }
}
