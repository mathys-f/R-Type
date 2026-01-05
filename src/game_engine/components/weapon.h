#pragma once

#include <cstdint>
#include <string>

#include "components/i_sync_component.h"

namespace engn::cpnt {

struct Weapon : ISyncComponent {
    int cooldown;

    int firerate;

    int dmg;

    // Initial velocity for spawned projectiles (2D)
    float vx;
    float vy;

    std::string mesh;
};

} // namespace engn::cpnt
