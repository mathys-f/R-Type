#pragma once

#include "components/i_sync_component.h"

namespace engn::cpnt {

struct Player : ISyncComponent {
    std::uint8_t id{0};

    Player() = default;
    Player(std::uint8_t player_id);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt