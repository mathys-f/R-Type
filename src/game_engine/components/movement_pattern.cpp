#include "components/movement_pattern.h"
#include <cstring>

using namespace engn::cpnt;

engn::SerializedComponent MovementPattern::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::movement_pattern;
    serialized.size = sizeof(MovementPattern);
    serialized.data.resize(sizeof(MovementPattern));
    std::memcpy(serialized.data.data(), this, sizeof(MovementPattern));
    return serialized;
}

void MovementPattern::deserialize(const std::vector<std::byte>& data) {
    if (data.size() >= sizeof(MovementPattern)) {
        std::memcpy(this, data.data(), sizeof(MovementPattern));
    }
}