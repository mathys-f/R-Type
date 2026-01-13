#include "snapshots.h"

#include <cstring>

using namespace engn;

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
