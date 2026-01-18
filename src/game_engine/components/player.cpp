#include "components/player.h"
#include <cstring>

using namespace engn::cpnt;

Player::Player(std::uint8_t player_id)
    : id(player_id) {};

// NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
engn::SerializedComponent Player::serialize() const {
    engn::SerializedComponent serialized;
    serialized.type = engn::ComponentType::player;
    std::uint32_t size = sizeof(id) + sizeof(shoot_cooldown);
    serialized.data.resize(size);

    std::size_t offset = 0;
    std::memcpy(serialized.data.data() + offset, &id, sizeof(id));
    offset += sizeof(id);
    std::memcpy(serialized.data.data() + offset, &shoot_cooldown, sizeof(shoot_cooldown));

    return serialized;
}

void Player::deserialize(const std::vector<std::byte>& data) {
    std::uint32_t size = sizeof(id) + sizeof(shoot_cooldown);

    if (data.size() >= size) {
        std::size_t offset = 0;
        std::memcpy(&id, data.data() + offset, sizeof(id));
        offset += sizeof(id);
        std::memcpy(&shoot_cooldown, data.data() + offset, sizeof(shoot_cooldown));
    }
}
// NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
