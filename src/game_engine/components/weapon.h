#pragma once

#include <string>
#include <cstdint>


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
};

} // namespace component

} // namespace engine

