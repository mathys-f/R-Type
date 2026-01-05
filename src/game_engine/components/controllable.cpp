#include "components/controllable.h"
#include <cstring>

using namespace engn::cpnt;

engn::SerializedComponent Controllable::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::controllable;
    serialized.size = sizeof(Controllable);
    serialized.data.resize(sizeof(Controllable));
    std::memcpy(serialized.data.data(), this, sizeof(Controllable));
    return serialized;
}

void Controllable::deserialize(const std::vector<std::byte>& data) {
    if (data.size() >= sizeof(Controllable)) {
        std::memcpy(this, data.data(), sizeof(Controllable));
    }
}