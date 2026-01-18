#pragma once

#include "components/components.h"
#include "ecs/sparse_array.h"

namespace engn {

class EngineContext;

namespace sys {

void fetch_inputs(EngineContext& ctx);
void log_inputs(EngineContext& ctx);
void resolve_player_input(EngineContext& ctx);

void ui_hover(EngineContext& ctx, const ecs::SparseArray<cpnt::UITransform>&);
void ui_navigation(EngineContext& ctx, const ecs::SparseArray<cpnt::UIInteractable>&,
                   const ecs::SparseArray<cpnt::UIFocusable>&, const ecs::SparseArray<cpnt::UINavigation>&);
void ui_press(EngineContext& ctx);
void ui_background_renderer(EngineContext& ctx, const ecs::SparseArray<cpnt::UITransform>&,
                            const ecs::SparseArray<cpnt::UIStyle>&, const ecs::SparseArray<cpnt::UIInteractable>&);
void ui_text_renderer(EngineContext& ctx, const ecs::SparseArray<cpnt::UITransform>&,
                      const ecs::SparseArray<cpnt::UIText>&, const ecs::SparseArray<cpnt::UIStyle>&,
                      const ecs::SparseArray<cpnt::UIInteractable>&);
void ui_input_field_updater(EngineContext& ctx, const ecs::SparseArray<cpnt::UIInteractable>&);

void bullet_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                   ecs::SparseArray<cpnt::Velocity> const& velocities, ecs::SparseArray<cpnt::Bullet> const& bullets);

void bullet_shooter_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                   ecs::SparseArray<cpnt::Velocity> const& velocities, ecs::SparseArray<cpnt::Bullet_shooter> const& bullets_shooter);

void collision_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                      ecs::SparseArray<cpnt::Bullet> const& bullets, ecs::SparseArray<cpnt::Enemy> const& enemies,
                      ecs::SparseArray<cpnt::Health> const& healths, ecs::SparseArray<cpnt::Player> const& players,
                      ecs::SparseArray<cpnt::Hitbox> const& hitboxes, ecs::SparseArray<cpnt::Bullet_shooter> const& bullets_shooter,
                      ecs::SparseArray<cpnt::Shooter> const& shooters, ecs::SparseArray<cpnt::Stats> const& stats);

void enemy_movement_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                           ecs::SparseArray<cpnt::MovementPattern> const& patterns,
                           ecs::SparseArray<cpnt::Velocity> const& velocity);

void enemy_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                  ecs::SparseArray<cpnt::Velocity> const& velocities, ecs::SparseArray<cpnt::Enemy> const& enemies,
                  ecs::SparseArray<cpnt::Health> const& healths, ecs::SparseArray<cpnt::Sprite> const& sprites);

void shooter_movement_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                           ecs::SparseArray<cpnt::MovementPattern> const& patterns,
                           ecs::SparseArray<cpnt::Velocity> const& velocity,
                           ecs::SparseArray<cpnt::Shooter> const& shooters,
                           ecs::SparseArray<cpnt::Player> const& player);

void shooter_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                  ecs::SparseArray<cpnt::Velocity> const& velocities,
                  ecs::SparseArray<cpnt::Health> const& healths, ecs::SparseArray<cpnt::Sprite> const& sprites,
                  ecs::SparseArray<cpnt::Shooter> const& shooters, ecs::SparseArray<cpnt::Player> const& player);

void explosion_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                      ecs::SparseArray<cpnt::Explosion> const& explosions,
                      ecs::SparseArray<cpnt::Sprite> const& sprites);

void particle_emission_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                              ecs::SparseArray<cpnt::Velocity> const& velocities,
                              ecs::SparseArray<cpnt::Particle> const& particles,
                              ecs::SparseArray<cpnt::Bullet> const& bullets,
                              ecs::SparseArray<cpnt::Bullet_shooter> const& bullets_shooter);

void player_control_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                           ecs::SparseArray<cpnt::Player> const& players, ecs::SparseArray<cpnt::Sprite> const& sprites,
                           ecs::SparseArray<cpnt::Velocity> const& velocities,
                           ecs::SparseArray<cpnt::Health> const& healths);

void render_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                   ecs::SparseArray<cpnt::Sprite> const& sprites, ecs::SparseArray<cpnt::Star> const& stars,
                   ecs::SparseArray<cpnt::Velocity> const& velocities,
                   ecs::SparseArray<cpnt::Particle> const& particles);

void star_scroll_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
                        ecs::SparseArray<cpnt::Star> const& stars);


void stat_system(EngineContext& ctx, ecs::SparseArray<cpnt::Stats> const& stats);

// In order to use the networking feature of the engine, these three systems must be called in this order:
// 1. poll_input_events_from_client
// 2. create_snapshot_system
// 3. update_snapshots_system
// 4. send_snapshot_to_client_system
// 5. clear_tombstones_system
void poll_input_events_from_client(EngineContext& ctx);
void create_snapshot_system(EngineContext& ctx, ecs::SparseArray<cpnt::Replicated> const& replicated_components);
void update_snapshots_system(EngineContext& ctx);
void send_snapshot_to_client_system(EngineContext& ctx, ecs::SparseArray<cpnt::Replicated> const& replicated_components);
void clear_tombstones_system(EngineContext &ctx);


void handle_snapshots_deltas_system(EngineContext& ctx);
void send_input_events_to_server(EngineContext& ctx);

} // namespace sys

} // namespace engn
