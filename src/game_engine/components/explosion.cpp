#include "components/explosion.h"
#include <cstring>

using namespace engn::cpnt;

engn::SerializedComponent Explosion::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::explosion;
    serialized.size = sizeof(Explosion);
    serialized.data.resize(sizeof(Explosion));
    std::memcpy(serialized.data.data(), this, sizeof(Explosion));
    return serialized;
}

void Explosion::deserialize(const std::vector<std::byte>& data) {
    if (data.size() >= sizeof(Explosion)) {
        std::memcpy(this, data.data(), sizeof(Explosion));
    }
}