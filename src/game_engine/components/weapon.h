#pragma once

#include "snapshot_meta.h"

#include <cstdint>
#include <string>

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

} // namespace cpnt

} // namespace engn
