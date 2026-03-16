#include "snapshots.h"

#include <cstring>

using namespace engn;

// NOLINTBEGIN (cppcoreguidelines-pro-bounds-pointer-arithmetic)

#pragma region Component

/**
 * Component memory layout:
 * [ComponentType : uint8]
 * [data size : uint16]  // Should be enough for all component data (65kB)
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

#pragma region WorldDelta

/**
 * WorldDelta memory layout:
 * [ base_snapshot_tick : uint32 ]
 * [ entry_count : uint32 ]
 * for i in 1..entry_count:
 *     [ op : uint8 ]
 *     [ entity_id : uint32 ]
 *     if op == EntityAdd:
 *         // no data
 *     if op == EntityRemove:
 *         // no data
 *     if op == ComponentAddOrUpdate:
 *         [ component_type : uint16 ]  // optional if part of SerializedComponent
 *         [ component_size : uint32 ]
 *         [ component_data... ]
 *     if op == ComponentRemove:
 *         [ component_type : uint16 ]
 */

 std::byte* engn::WorldDelta::serialize() const {
    uint32_t total_size = get_serialized_size();

    std::byte *buffer = new std::byte[total_size]; // NOLINT(cppcoreguidelines-owning-memory)
    std::byte *ptr = buffer;

    // base_snapshot_tick
    std::memcpy(ptr, &base_snapshot_tick, sizeof(std::uint32_t));
    ptr += sizeof(std::uint32_t);

    // number of entries
    std::uint32_t num_entries = static_cast<std::uint32_t>(entries.size());
    std::memcpy(ptr, &num_entries, sizeof(std::uint32_t));
    ptr += sizeof(std::uint32_t);

    // entries
    for (const auto &entry : entries) {
        // operation
        std::uint8_t operation = static_cast<std::uint8_t>(entry.operation);
        std::memcpy(ptr, &operation, sizeof(std::uint8_t));
        ptr += sizeof(std::uint8_t);

        // entity_id
        std::memcpy(ptr, &entry.entity_id, sizeof(std::uint32_t));
        ptr += sizeof(std::uint32_t);

        switch (entry.operation) {
            case DeltaOperation::entity_add: break;
            case DeltaOperation::entity_remove: break;

            case DeltaOperation::component_add_or_update: {
                SerializedComponent component = entry.component.value();

                std::byte *component_data = component.serialize();
                std::uint32_t component_size = component.get_serialized_size();
                std::memcpy(ptr, component_data, component_size);
                ptr += component_size;
                delete[] component_data; // NOLINT(cppcoreguidelines-owning-memory)
                break;
            }

            case DeltaOperation::component_remove: {
                std::memcpy(ptr, &entry.component_type, sizeof(ComponentType));
                ptr += sizeof(ComponentType);
                break;
            }
        }
    }

    return buffer;
}

WorldDelta engn::WorldDelta::deserialize(const std::byte *data_ptr) {
    WorldDelta delta;
    const std::byte *ptr = data_ptr;

    // base_snapshot_tick
    std::memcpy(&delta.base_snapshot_tick, ptr, sizeof(std::uint32_t));
    ptr += sizeof(std::uint32_t);

    // number of entries
    std::uint32_t num_entries = 0;
    std::memcpy(&num_entries, ptr, sizeof(std::uint32_t));
    ptr += sizeof(std::uint32_t);

    // entries
    for (std::uint32_t i = 0; i < num_entries; ++i) {
        DeltaEntry entry;

        // operation
        std::uint8_t operation = 0;
        std::memcpy(&operation, ptr, sizeof(std::uint8_t));
        entry.operation = static_cast<DeltaOperation>(operation);
        ptr += sizeof(std::uint8_t);

        // entity_id
        std::memcpy(&entry.entity_id, ptr, sizeof(std::uint32_t));
        ptr += sizeof(std::uint32_t);

        switch (entry.operation) {
            case DeltaOperation::entity_add: break;
            case DeltaOperation::entity_remove: break;

            case DeltaOperation::component_add_or_update: {
                entry.component = SerializedComponent::deserialize(ptr);
                ptr += entry.component->get_serialized_size();
                break;
            }

            case DeltaOperation::component_remove: {
                std::memcpy(&entry.component_type, ptr, sizeof(ComponentType));
                ptr += sizeof(ComponentType);
                break;
            }
        }

        delta.entries.push_back(entry);
    }

    return delta;
}

std::uint32_t engn::WorldDelta::get_serialized_size() const {
    std::uint32_t total_size = 0;
    total_size += sizeof(std::uint32_t); // base_snapshot_tick
    total_size += sizeof(std::uint32_t); // number of entries

    for (const auto &entry : entries) {
        total_size += sizeof(std::uint8_t); // operation
        total_size += sizeof(std::uint32_t); // entity_id

        switch (entry.operation) {
            case DeltaOperation::entity_add: break;
            case DeltaOperation::entity_remove: break;

            case DeltaOperation::component_add_or_update: {
                total_size += entry.component->get_serialized_size();
                break;
            }

            case DeltaOperation::component_remove: {
                total_size += sizeof(ComponentType);
                break;
            }
        }
    }

    return total_size;
}

#pragma endregion WorldDelta

// NOLINTEND (cppcoreguidelines-pro-bounds-pointer-arithmetic)
