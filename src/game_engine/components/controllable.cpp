#include "components/controllable.h"
#include <cstring>

using namespace engn::cpnt;

Controllable::Controllable(float speed) : speed(speed) {}

engn::SerializedComponent Controllable::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::controllable;
    serialized.data.resize(sizeof(speed));
    std::memcpy(serialized.data.data(), &speed, serialized.data.size());
    return serialized;
}

void Controllable::deserialize(const std::vector<std::byte>& data) {
    if (data.size() >= sizeof(speed)) {
        std::memcpy(&speed, data.data(), sizeof(speed));
    }
}