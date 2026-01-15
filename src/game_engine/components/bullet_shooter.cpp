#include "components/bullet_shooter.h"

using namespace engn::cpnt;

engn::SerializedComponent Bullet_shooter::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::bullet_shooter;
    serialized.data = {}; // Empty data for tag component
    return serialized;
}

void Bullet_shooter::deserialize(const std::vector<std::byte>& data) {
    // Tag component has no data to deserialize
    // Function intentionally empty
}
