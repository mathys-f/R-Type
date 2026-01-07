#include "components/enemy.h"

using namespace engn::cpnt;

engn::SerializedComponent Enemy::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::enemy;
    serialized.size = 0; // Tag component has no data
    serialized.data = {}; // Empty data for tag component
    return serialized;
}

void Enemy::deserialize(const std::vector<std::byte>& data) {
    // Tag component has no data to deserialize
    // Function intentionally empty
}