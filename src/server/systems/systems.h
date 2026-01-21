#pragma once

#include "components/components.h"
#include "ecs/sparse_array.h"

namespace engn {

class EngineContext;

namespace sys {

void server_player_control_system(EngineContext& ctx,
    ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::Player> const& players,
    ecs::SparseArray<cpnt::Velocity> const& velocities);

void server_enemy_movement_system(EngineContext& ctx,
    ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::MovementPattern> const& patterns,
    ecs::SparseArray<cpnt::Velocity> const& velocity);

void server_enemy_system(EngineContext& ctx,
    ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::Velocity> const& velocities,
    ecs::SparseArray<cpnt::Enemy> const& enemies,
    ecs::SparseArray<cpnt::Health> const& healths);

void server_update_player_entities_system(EngineContext &ctx,
    ecs::SparseArray<cpnt::Player> const& players);

void server_bullet_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::Velocity> const& velocities,
    ecs::SparseArray<cpnt::Bullet> const& bullets);

void server_collision_system(EngineContext& ctx, ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::Bullet> const& bullets,
    ecs::SparseArray<cpnt::Enemy> const& enemies,
    ecs::SparseArray<cpnt::Health> const& healths,
    ecs::SparseArray<cpnt::Hitbox> const& hitboxes,
    ecs::SparseArray<cpnt::Player> const& players);

} // namespace sys

} // namespace engn
