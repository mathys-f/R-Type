#pragma once

#include "components/i_sync_component.h"

namespace engn::cpnt {

struct Bullet_shooter : ISyncComponent {
    // Tag component for bullet_shooters

    Bullet_shooter() = default;

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
