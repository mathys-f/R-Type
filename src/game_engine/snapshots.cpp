#include "snapshots.h"

#include <cstring>

using namespace engn;

#pragma region Component

/**
 * Component memory layout:
 * [ComponentType (1 byte)]
 * [data size (2 bytes)]  // Should be enough for all component data (65kB)
 * [data (data size bytes)]
 */

std::byte *SerializedComponent::serialize() const {
    uint32_t total_size = sizeof(ComponentType) + sizeof(uint16_t) + data.size();
    std::byte *buffer = new std::byte[total_size]; // NOLINT(cppcoreguidelines-owning-memory)

    // ComponentType
    std::memcpy(buffer, &type, sizeof(ComponentType));

    // Data size
    uint16_t data_size = static_cast<uint16_t>(data.size());
    std::memcpy(buffer + sizeof(ComponentType), &data_size, sizeof(uint16_t)); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    // Data
    std::memcpy(buffer + sizeof(ComponentType) + sizeof(uint16_t), data.data(), data.size()); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    return buffer;
}

SerializedComponent SerializedComponent::deserialize(std::byte const* data_ptr)
{
    SerializedComponent component;

    // ComponentType
    std::memcpy(&component.type, data_ptr, sizeof(ComponentType));

    // Data size
    uint16_t data_size = 0;
    std::memcpy(&data_size, data_ptr + sizeof(ComponentType), sizeof(uint16_t)); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    // Data
    component.data.resize(data_size);
    std::memcpy(component.data.data(),
                data_ptr + sizeof(ComponentType) + sizeof(uint16_t), // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                data_size);

    return component;
}

std::uint32_t SerializedComponent::get_serialized_size() const
{
    return sizeof(ComponentType) + sizeof(uint16_t) + data.size();
}

#pragma endregion Component

#pragma region Entity

/**
 * Entity memory layout:
 * [entity_id (4 bytes)]
 * [number of components (2 bytes)]
 * [components...]
 */

std::byte *EntitySnapshot::serialize() const {
    uint32_t total_size = sizeof(entity_id) + sizeof(uint16_t);
    for (const auto &component : components) {
        total_size += component.get_serialized_size();
    }
    std::byte *buffer = new std::byte[total_size]; // NOLINT(cppcoreguidelines-owning-memory)

    // Entity ID
    std::memcpy(buffer, &entity_id, sizeof(entity_id));

    // Component count
    uint16_t component_count = static_cast<uint16_t>(components.size());
    std::memcpy(buffer + sizeof(entity_id), &component_count, sizeof(uint16_t)); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    // Components
    std::byte *current_ptr = buffer + sizeof(entity_id) + sizeof(uint16_t); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    for (const auto &component : components) {
        std::byte *component_data = component.serialize(); // NOLINT(cppcoreguidelines-owning-memory)
        std::uint32_t component_size = component.get_serialized_size();
        std::memcpy(current_ptr, component_data, component_size);
        current_ptr += component_size; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        delete[] component_data; // NOLINT(cppcoreguidelines-owning-memory)
    }

    return buffer;
}

EntitySnapshot EntitySnapshot::deserialize(std::byte const* data_ptr)
{
    EntitySnapshot entity_snapshot;

    // Entity ID
    std::memcpy(&entity_snapshot.entity_id, data_ptr, sizeof(entity_snapshot.entity_id));

    // Component count
    uint16_t component_count = 0;
    std::memcpy(&component_count, data_ptr + sizeof(entity_snapshot.entity_id), sizeof(uint16_t)); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    // Components
    std::byte const* current_ptr = data_ptr + sizeof(entity_snapshot.entity_id) + sizeof(uint16_t); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    for (uint16_t i = 0; i < component_count; ++i) {
        SerializedComponent component = SerializedComponent::deserialize(current_ptr);
        entity_snapshot.components.push_back(component);
        current_ptr += component.get_serialized_size(); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }

    return entity_snapshot;
}

std::uint32_t EntitySnapshot::get_serialized_size() const
{
    uint32_t total_size = sizeof(entity_id) + sizeof(uint16_t);
    for (const auto &component : components) {
        total_size += component.get_serialized_size();
    }
    return total_size;
}

#pragma endregion Entity

#pragma region World

/**
 * World memory layout:
 * [number of entities (4 bytes)]
 * [entities...]
 */

std::byte *WorldSnapshot::serialize() const {
    uint32_t total_size = sizeof(uint32_t);
    for (const auto &entity : entities) {
        total_size += entity.get_serialized_size();
    }
    std::byte *buffer = new std::byte[total_size]; // NOLINT(cppcoreguidelines-owning-memory)

    // Entity count
    uint32_t entity_count = static_cast<uint32_t>(entities.size());
    std::memcpy(buffer, &entity_count, sizeof(uint32_t));

    // Entities
    std::byte *current_ptr = buffer + sizeof(uint32_t); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    for (const auto &entity : entities) {
        std::byte *entity_data = entity.serialize(); // NOLINT(cppcoreguidelines-owning-memory)
        std::uint32_t entity_size = entity.get_serialized_size();
        std::memcpy(current_ptr, entity_data, entity_size);
        current_ptr += entity_size; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        delete[] entity_data; // NOLINT(cppcoreguidelines-owning-memory)
    }

    return buffer;
}

WorldSnapshot WorldSnapshot::deserialize(std::byte const* data_ptr)
{
    WorldSnapshot world_snapshot;

    // Entity count
    uint32_t entity_count = 0;
    std::memcpy(&entity_count, data_ptr, sizeof(uint32_t));

    // Entities
    std::byte const* current_ptr = data_ptr + sizeof(uint32_t); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    for (uint32_t i = 0; i < entity_count; ++i) {
        EntitySnapshot entity = EntitySnapshot::deserialize(current_ptr);
        world_snapshot.entities.push_back(entity);
        current_ptr += entity.get_serialized_size(); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }

    return world_snapshot;
}

#pragma endregion World
