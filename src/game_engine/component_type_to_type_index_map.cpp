#include "components/boss.h"
#include "components/boss_hitbox.h"
#include "components/bullet.h"
#include "components/bullet_shooter.h"
#include "components/controllable.h"
#include "components/enemy.h"
#include "components/entity_type.h"
#include "components/health.h"
#include "components/hitbox.h"
#include "components/movement_pattern.h"
#include "components/player.h"
#include "components/replicated.h"
#include "components/shooter.h"
#include "components/sprite.h"
#include "components/stats.h"
#include "components/tag.h"
#include "components/transform.h"
#include "components/velocity.h"
#include "snapshots.h"

using namespace engn;

const std::unordered_map<ComponentType, std::type_index> engn::k_component_type_to_type_index_map = {
    {ComponentType::Bullet, typeid(cpnt::Bullet)},
    {ComponentType::Controllable, typeid(cpnt::Controllable)},
    {ComponentType::Enemy, typeid(cpnt::Enemy)},
    {ComponentType::EntityType, typeid(cpnt::EntityType)},
    {ComponentType::Health, typeid(cpnt::Health)},
    {ComponentType::Hitbox, typeid(cpnt::Hitbox)},
    {ComponentType::Player, typeid(cpnt::Player)},
    {ComponentType::Replicated, typeid(cpnt::Replicated)},
    {ComponentType::MovementPattern, typeid(cpnt::MovementPattern)},
    {ComponentType::Shooter, typeid(cpnt::Shooter)},
    {ComponentType::Stats, typeid(cpnt::Stats)},
    {ComponentType::Tag, typeid(cpnt::Tag)},
    {ComponentType::Transform, typeid(cpnt::Transform)},
    {ComponentType::Velocity, typeid(cpnt::Velocity)},
    {ComponentType::BulletShooter, typeid(cpnt::BulletShooter)},
    {ComponentType::Boss, typeid(cpnt::Boss)},
    {ComponentType::BossHitbox, typeid(cpnt::BossHitbox)}};

const std::unordered_map<std::type_index, ComponentType> engn::k_type_index_to_component_type_map = {
    {typeid(cpnt::Bullet), ComponentType::Bullet},
    {typeid(cpnt::Controllable), ComponentType::Controllable},
    {typeid(cpnt::Enemy), ComponentType::Enemy},
    {typeid(cpnt::EntityType), ComponentType::EntityType},
    {typeid(cpnt::Health), ComponentType::Health},
    {typeid(cpnt::Hitbox), ComponentType::Hitbox},
    {typeid(cpnt::Player), ComponentType::Player},
    {typeid(cpnt::Replicated), ComponentType::Replicated},
    {typeid(cpnt::MovementPattern), ComponentType::MovementPattern},
    {typeid(cpnt::Shooter), ComponentType::Shooter},
    {typeid(cpnt::Stats), ComponentType::Stats},
    {typeid(cpnt::Tag), ComponentType::Tag},
    {typeid(cpnt::Transform), ComponentType::Transform},
    {typeid(cpnt::Velocity), ComponentType::Velocity},
    {typeid(cpnt::BulletShooter), ComponentType::BulletShooter},
    {typeid(cpnt::Boss), ComponentType::Boss},
    {typeid(cpnt::BossHitbox), ComponentType::BossHitbox}};
