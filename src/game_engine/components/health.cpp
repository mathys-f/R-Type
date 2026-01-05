#include "components/health.h"
#include <cstring>

using namespace engn::cpnt;

engn::SerializedComponent Health::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::health;
    serialized.size = sizeof(hp) + sizeof(max_hp) + sizeof(changes);
    serialized.data.resize(serialized.size);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &hp, sizeof(hp));
    offset += sizeof(hp);
    std::memcpy(serialized.data.data() + offset, &max_hp, sizeof(max_hp));
    offset += sizeof(max_hp);
    std::memcpy(serialized.data.data() + offset, &changes, sizeof(changes));
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return serialized;
}

void Health::deserialize(const std::vector<std::byte>& data) {
    std::uint16_t size = sizeof(hp) + sizeof(max_hp) + sizeof(changes);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (data.size() >= size) {
        std::size_t offset = 0;
        std::memcpy(&hp, data.data() + offset, sizeof(hp));
        offset += sizeof(hp);
        std::memcpy(&max_hp, data.data() + offset, sizeof(max_hp));
        offset += sizeof(max_hp);
        std::memcpy(&changes, data.data() + offset, sizeof(changes));
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}