#pragma once

#include "components/i_sync_component.h"

#include <cstring>

namespace engn::cpnt {

struct Player : ISyncComponent {
    std::uint8_t id{0};

    Player() = default;

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
