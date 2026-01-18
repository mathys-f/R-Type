#include "snapshots.h"

#include "components/bullet.h"
#include "components/controllable.h"
#include "components/enemy.h"
#include "components/entity_type.h"
#include "components/health.h"
#include "components/hitbox.h"
#include "components/movement_pattern.h"
#include "components/player.h"
#include "components/replicated.h"
#include "components/sprite.h"
#include "components/stats.h"
#include "components/tag.h"
#include "components/transform.h"
#include "components/velocity.h"

using namespace engn;

const std::unordered_map<ComponentType, std::type_index> engn::k_component_type_to_type_index_map = {
    {ComponentType::bullet, typeid(cpnt::Bullet)},
    {ComponentType::controllable, typeid(cpnt::Controllable)},
    {ComponentType::enemy, typeid(cpnt::Enemy)},
    {ComponentType::entity_type, typeid(cpnt::EntityType)},
    {ComponentType::health, typeid(cpnt::Health)},
    {ComponentType::hitbox, typeid(cpnt::Hitbox)},
    {ComponentType::player, typeid(cpnt::Player)},
    {ComponentType::replicated, typeid(cpnt::Replicated)},
    {ComponentType::movement_pattern, typeid(cpnt::MovementPattern)},
    {ComponentType::stats, typeid(cpnt::Stats)},
    {ComponentType::tag, typeid(cpnt::Tag)},
    {ComponentType::transform, typeid(cpnt::Transform)},
    {ComponentType::velocity, typeid(cpnt::Velocity)}
};

const std::unordered_map<std::type_index, ComponentType> engn::k_type_index_to_component_type_map = {
    {typeid(cpnt::Bullet), ComponentType::bullet},
    {typeid(cpnt::Controllable), ComponentType::controllable},
    {typeid(cpnt::Enemy), ComponentType::enemy},
    {typeid(cpnt::EntityType), ComponentType::entity_type},
    {typeid(cpnt::Health), ComponentType::health},
    {typeid(cpnt::Hitbox), ComponentType::hitbox},
    {typeid(cpnt::Player), ComponentType::player},
    {typeid(cpnt::Replicated), ComponentType::replicated},
    {typeid(cpnt::MovementPattern), ComponentType::movement_pattern},
    {typeid(cpnt::Stats), ComponentType::stats},
    {typeid(cpnt::Tag), ComponentType::tag},
    {typeid(cpnt::Transform), ComponentType::transform},
    {typeid(cpnt::Velocity), ComponentType::velocity}
};
