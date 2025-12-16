#pragma once

#include <string>
#include <cstdint>
#include "snapshot_meta.h"


namespace engn {

namespace cpnt {

struct Weapon {
    int cooldown;

    int firerate;

    int dmg;

    // Initial velocity for spawned projectiles (2D)
    float vx;
    float vy;

    std::string mesh;

    SnapshotMeta snapshot_meta;
};

} // namespace component

} // namespace engine

