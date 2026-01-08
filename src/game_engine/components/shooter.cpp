#include "components/shooter.h"

using namespace engn::cpnt;

engn::SerializedComponent Shooter::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::shooter;
    serialized.size = 0; // Tag component has no data
    serialized.data = {}; // Empty data for tag component
    return serialized;
}

void Shooter::deserialize(const std::vector<std::byte>& data) {
    // Tag component has no data to deserialize
    // Function intentionally empty
}