#pragma once

#include <any>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace engn {

enum class ComponentType : std::uint8_t {
    Bullet,
    Controllable,
    Enemy,
    Shooter,
    Health,
    Hitbox,
    Player,
    Replicated,
    MovementPattern,
    Stats,
    Tag,
    Transform,
    Velocity,
    BulletShooter,
    Boss,
    BossHitbox,
    EntityType
};

extern const std::unordered_map<std::type_index, ComponentType> k_type_index_to_component_type_map;
extern const std::unordered_map<ComponentType, std::type_index> k_component_type_to_type_index_map;

struct SerializedComponent {
    ComponentType type;
    // Cannot use std::any here because it does not translate to a contiguous byte array.
    std::vector<std::byte> data;

    std::byte* serialize() const;
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
    std::uint32_t msg_id{0};
    std::uint32_t last_update_tick{0};
};

enum class DeltaOperation : std::uint8_t { EntityAdd, EntityRemove, ComponentAddOrUpdate, ComponentRemove };

struct DeltaEntry {
    DeltaOperation operation{DeltaOperation::EntityAdd};
    std::uint32_t entity_id{0};

    ComponentType component_type{ComponentType::Bullet}; // Only used for component remove
    std::optional<SerializedComponent> component;        // Only used for component add or update
};

struct WorldDelta {
    std::uint32_t base_snapshot_tick = 0; // Initialize with default value
    std::vector<DeltaEntry> entries;

    std::byte* serialize() const;
    static WorldDelta deserialize(const std::byte* data_ptr);
    std::uint32_t get_serialized_size() const;
};

} // namespace engn
