#pragma once

#include "components/i_sync_component.h"

namespace engn::cpnt {

struct Player : ISyncComponent {
    // Tag component for players

    Player() = default;

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
