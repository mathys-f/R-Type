#include "components/bullet_shooter.h"

using namespace engn::cpnt;

engn::SerializedComponent BulletShooter::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::BulletShooter;
    serialized.data = {}; // Empty data for tag component
    return serialized;
}

void BulletShooter::deserialize(const std::vector<std::byte>& data) {
    // Tag component has no data to deserialize
    // Function intentionally empty
}
