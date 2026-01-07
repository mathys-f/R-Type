#pragma once

#include <cstdint>
#include <any>
#include <vector>
#include <unordered_map>
#include <typeindex>

namespace engn {

enum ComponentType : std::uint8_t {
    bullet,
    controllable,
    enemy,
    health,
    hitbox,
    player,
    replicated,
    movement_pattern,
    stats,
    tag,
    transform,
    velocity
};

extern const std::unordered_map<std::type_index, ComponentType> k_type_index_to_component_type_map;
extern const std::unordered_map<ComponentType, std::type_index> k_component_type_to_type_index_map;

struct SerializedComponent {
    ComponentType type;
    std::uint32_t size;
    // Cannot use std::any here because it does not translate to a contiguous byte array.
    std::vector<std::byte> data;
};

struct EntitySnapshot {
    std::uint32_t entity_id;
    std::vector<SerializedComponent> components;
};

// This structure is what is sent over the network
struct WorldSnapshot {
    std::vector<EntitySnapshot> entities;
};

// This wrapper adds metadata to the WorldSnapshot
struct SnapshotRecord {
    WorldSnapshot snapshot;
    bool acknowledged = false;
    std::uint32_t last_update_tick;
};

} // namespace engn
