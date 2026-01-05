#include "components/hitbox.h"
#include <cstring>

using namespace engn::cpnt;

engn::SerializedComponent Hitbox::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::hitbox;
    serialized.size = sizeof(Hitbox);
    serialized.data.resize(sizeof(Hitbox));
    std::memcpy(serialized.data.data(), this, sizeof(Hitbox));
    return serialized;
}

void Hitbox::deserialize(const std::vector<std::byte>& data) {
    if (data.size() >= sizeof(Hitbox)) {
        std::memcpy(this, data.data(), sizeof(Hitbox));
    }
}