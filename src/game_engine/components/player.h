#pragma once

#include "components/i_sync_component.h"

#include <cstring>

namespace engn::cpnt {

inline constexpr float k_default_shoot_cooldown = 0.2f;

struct Player : ISyncComponent {
    std::uint8_t id{0};
    float shoot_cooldown{k_default_shoot_cooldown};

    Player() = default;
    Player(std::uint8_t player_id);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt