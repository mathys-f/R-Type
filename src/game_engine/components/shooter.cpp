#include "components/shooter.h"
#include <cstring>

using namespace engn::cpnt;

Shooter::Shooter(float time) : timer(time) {}

engn::SerializedComponent Shooter::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::shooter;
    serialized.size = sizeof(timer);
    serialized.data.resize(serialized.size);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &timer, sizeof(timer));
    offset += sizeof(timer);
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return serialized;
}

void Shooter::deserialize(const std::vector<std::byte>& data) {
    std::uint16_t size = sizeof(timer);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    if (data.size() >= size) {
        std::size_t offset = 0;
        std::memcpy(&timer, data.data() + offset, sizeof(timer));
        offset += sizeof(timer);
    }
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
}