#include "components/player.h"
#include <cstring>

using namespace engn::cpnt;

Player::Player(std::uint8_t player_id) : id(player_id) {}

engn::SerializedComponent Player::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::player;
    serialized.data.resize(sizeof(id));
    std::memcpy(serialized.data.data(), &id, sizeof(id));
    return serialized;
}

void Player::deserialize(const std::vector<std::byte>& data) {
    if (data.size() >= sizeof(id)) {
        std::memcpy(&id, data.data(), sizeof(id));
    }
}