#pragma once

#include <any>

#include "bullet.h"
#include "controllable.h"
#include "enemy.h"
#include "explosion.h"
#include "health.h"
#include "hitbox.h"
#include "movement_pattern.h"
#include "player.h"
#include "replicated.h"
#include "stats.h"
#include "tag.h"
#include "transform.h"
#include "velocity.h"
#include "weapon.h"
#include "sprite.h"

namespace engn::cpnt {

namespace type {
    
enum ComponentType {
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
    ComponentType type;
    std::any data;
};

} // namespace engn::cpnt
