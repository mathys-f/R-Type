#include "components/stats.h"
#include <cstring>

using namespace engn::cpnt;

engn::SerializedComponent Stats::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::stats;
    serialized.size = sizeof(score) + sizeof(dmg) + sizeof(kills);
    serialized.data.resize(serialized.size);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &score, sizeof(score));
    offset += sizeof(score);
    std::memcpy(serialized.data.data() + offset, &dmg, sizeof(dmg));
    offset += sizeof(dmg);
    std::memcpy(serialized.data.data() + offset, &kills, sizeof(kills));
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return serialized;
}

void Stats::deserialize(const std::vector<std::byte>& data) {
    std::uint16_t size = sizeof(score) + sizeof(dmg) + sizeof(kills);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (data.size() >= size) {
        std::size_t offset = 0;
        std::memcpy(&score, data.data() + offset, sizeof(score));
        offset += sizeof(score);
        std::memcpy(&dmg, data.data() + offset, sizeof(dmg));
        offset += sizeof(dmg);
        std::memcpy(&kills, data.data() + offset, sizeof(kills));
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}