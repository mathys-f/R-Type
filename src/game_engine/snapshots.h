#pragma once

#include <cstdint>
#include <any>
#include <vector>

namespace engn {

enum ComponentType : std::uint8_t {
    bullet,
    controllable,
    enemy,
    explosion,
    health,
    hitbox,
    player,
    replicated,
    movement_pattern,
    stats,
    tag,
    transform,
    velocity,
    weapon
};

struct SerializedComponent {
    ComponentType type;
    std::uint8_t size; // data size in bytes
    // Cannot use std::any here because it does not translates to a contiguous byte array.
    std::vector<std::byte> data;
};

struct EntitySnapshot {
    std::uint32_t entity_id;
    std::vector<SerializedComponent> components;
};

struct WorldSnapshot {
    std::uint32_t last_update_tick;
    std::vector<EntitySnapshot> entities;
};

} // namspace engn
