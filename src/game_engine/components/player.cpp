#include "components/player.h"

using namespace engn::cpnt;

engn::SerializedComponent Player::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::player;
    serialized.data = {}; // Empty data for tag component
    return serialized;
}

void Player::deserialize(const std::vector<std::byte>& data) {
    // Tag component has no data to deserialize
    // Function intentionally empty
}