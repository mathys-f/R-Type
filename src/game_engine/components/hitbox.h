#pragma once

#include <cstdint>

namespace engn {

namespace cpnt {

enum class CollisionLayer {
    Ally,
    Enemy,
    AllyProjectile,
    EnemyProjectile,
    Pickup,
    Environment,
};

struct Hitbox {
    float x;
    float y;
    float z;

    float w;
    float h;
    float d;

    CollisionLayer collisionLayer = CollisionLayer::Environment;
};

} // namespace component

} // namespace engine