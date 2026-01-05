#pragma once

#include "components/bullet.h"
#include "components/controllable.h"
#include "components/enemy.h"
#include "components/explosion.h"
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
#include "components/weapon.h"

#include <any>

namespace engn::cpnt {

namespace type {

enum class ComponentType {
    Bullet,
    Controllable,
    Enemy,
    Explosion,
    Health,
    Hitbox,
    MovementPattern,
    Player,
    Replicated,
    Stats,
    Tag,
    Transform,
    Velocity,
    Weapon,
    Sprite
};

} // namespace type

struct ComponentPackage {
    uint32_t tag;
    type::ComponentType type;
    std::any data;
};

} // namespace engn::cpnt
