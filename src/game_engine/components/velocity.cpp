#include "components/velocity.h"
#include <cstring>

using namespace engn::cpnt;

engn::SerializedComponent Velocity::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::velocity;
    serialized.size = sizeof(Velocity);
    serialized.data.resize(sizeof(Velocity));
    std::memcpy(serialized.data.data(), this, sizeof(Velocity));
    return serialized;
}

void Velocity::deserialize(const std::vector<std::byte>& data) {
    if (data.size() >= sizeof(Velocity)) {
        std::memcpy(this, data.data(), sizeof(Velocity));
    }
}