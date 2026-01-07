#include "components/replicated.h"
#include <cstring>

using namespace engn::cpnt;

Replicated::Replicated(std::uint32_t tag, size_t last_update_tick)
    : tag(tag), last_update_tick(last_update_tick) {}

engn::SerializedComponent Replicated::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::replicated;
    serialized.size = sizeof(tag) + sizeof(last_update_tick);
    serialized.data.resize(serialized.size);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &tag, sizeof(tag));
    offset += sizeof(tag);
    std::memcpy(serialized.data.data() + offset, &last_update_tick, sizeof(last_update_tick));
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return serialized;
}

void Replicated::deserialize(const std::vector<std::byte>& data) {
    std::uint16_t size = sizeof(tag) + sizeof(last_update_tick);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (data.size() >= size) {
        std::size_t offset = 0;
        std::memcpy(&tag, data.data() + offset, sizeof(tag));
        offset += sizeof(tag);
        std::memcpy(&last_update_tick, data.data() + offset, sizeof(last_update_tick));
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}