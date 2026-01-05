#include "components/controllable.h"
#include <cstring>

using namespace engn::cpnt;

engn::SerializedComponent Controllable::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::controllable;
    serialized.size = sizeof(speed);
    serialized.data.resize(serialized.size);
    std::memcpy(serialized.data.data(), &speed, sizeof(speed));
    return serialized;
}

void Controllable::deserialize(const std::vector<std::byte>& data) {
    if (data.size() >= sizeof(speed)) {
        std::memcpy(&speed, data.data(), sizeof(speed));
    }
}