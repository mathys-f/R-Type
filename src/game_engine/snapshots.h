#pragma once

#include <cstdint>
#include <any>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <cstddef>
#include <optional>

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
    // Cannot use std::any here because it does not translate to a contiguous byte array.
    std::vector<std::byte> data;

    std::byte *serialize() const;
    static SerializedComponent deserialize(std::byte const* data_ptr);
    std::uint32_t get_serialized_size() const;
};

struct EntitySnapshot {
    std::uint32_t entity_id;
    std::vector<SerializedComponent> components;
};

struct WorldSnapshot {
    std::vector<EntitySnapshot> entities;
};

// This wrapper adds metadata to the WorldSnapshot
struct SnapshotRecord {
    WorldSnapshot snapshot;
    bool acknowledged = false;
    std::uint32_t last_update_tick;
};

enum class DeltaOperation : std::uint8_t {
    entity_add,
    entity_remove,
    component_add_or_update,
    component_remove
};

struct DeltaEntry {
    DeltaOperation operation;
    std::uint32_t entity_id;

    ComponentType component_type; // Only used for component remove
    std::optional<SerializedComponent> component; // Only used for component add or update
};

struct WorldDelta {
    std::uint32_t base_snapshot_tick;
    std::vector<DeltaEntry> entries;

    std::byte* serialize() const;
    static WorldDelta deserialize(const std::byte *data_ptr);
    std::uint32_t get_serialized_size() const;
};

} // namespace engn
