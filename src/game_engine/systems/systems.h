#pragma once

#include "ecs/sparse_array.h"
#include "../components/components.h"

namespace engn {

class EngineContext;

namespace sys {

void fetch_inputs(EngineContext &ctx);
void log_inputs(EngineContext &ctx);

void ui_hover(EngineContext &ctx,
    const ecs::SparseArray<cpnt::UITransform> &);
void ui_press(EngineContext &ctx);
void ui_background_renderer(EngineContext &ctx,
    const ecs::SparseArray<cpnt::UITransform> &,
    const ecs::SparseArray<cpnt::UIStyle> &
);
void ui_text_renderer(EngineContext &ctx,
    const ecs::SparseArray<cpnt::UITransform> &,
    const ecs::SparseArray<cpnt::UIText> &,
    const ecs::SparseArray<cpnt::UIStyle> &
);

void BulletSystem(EngineContext &ctx,
    ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::Velocity> const& velocities,
    ecs::SparseArray<cpnt::Bullet> const& bullets);

void CollisionSystem(EngineContext &ctx,
    ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::Bullet> const& bullets,
    ecs::SparseArray<cpnt::Enemy> const& enemies,
    ecs::SparseArray<cpnt::Health> const& healths,
    ecs::SparseArray<cpnt::Player> const& players,
    ecs::SparseArray<cpnt::Hitbox> const& hitboxes);

void EnemyMovementSystem(EngineContext &ctx,
    ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::MovementPattern> const& patterns,
    ecs::SparseArray<cpnt::Velocity> const& velocity);

void EnemySystem(EngineContext &ctx,
    ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::Velocity> const& velocities,
    ecs::SparseArray<cpnt::Enemy> const& enemies,
    ecs::SparseArray<cpnt::Health> const& healths,
    ecs::SparseArray<cpnt::Sprite> const& sprites);

void ExplosionSystem(EngineContext &ctx,
    ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::Explosion> const& explosions,
    ecs::SparseArray<cpnt::Sprite> const& sprites);

void ParticleEmissionSystem(EngineContext &ctx,
    ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::Velocity> const& velocities,
    ecs::SparseArray<cpnt::Particle> const& particles,
    ecs::SparseArray<cpnt::Bullet> const& bullets);

void PlayerControlSystem(EngineContext &ctx,
    ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::Player> const& players,
    ecs::SparseArray<cpnt::Sprite> const& sprites,
    ecs::SparseArray<cpnt::Velocity> const& velocities,
    ecs::SparseArray<cpnt::Health> const& healths,
    Texture2D& shipTexture,
    Sound& shootSound);

void RenderSystem(EngineContext &ctx,
    ecs::SparseArray<cpnt::Transform> const& positions,
    ecs::SparseArray<cpnt::Sprite> const& sprites,
    ecs::SparseArray<cpnt::Star> const& stars,
    ecs::SparseArray<cpnt::Velocity> const& velocities,
    ecs::SparseArray<cpnt::Particle> const& particles);
} // namespace system

} // namespace engine
