#include "components/bullet.h"

using namespace engn::cpnt;

engn::SerializedComponent Bullet::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::bullet;
    serialized.size = 0; // Tag component has no data
    serialized.data = {}; // Empty data for tag component
    return serialized;
}

void Bullet::deserialize(const std::vector<std::byte>& data) {
    // Tag component has no data to deserialize
    // Function intentionally empty
}
