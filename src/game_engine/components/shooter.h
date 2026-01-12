#pragma once

#include "components/i_sync_component.h"

namespace engn::cpnt {

struct Shooter : ISyncComponent {
    // Tag component for enemies

    float timer{};

    Shooter() = default;
    Shooter(float time = 0.0f);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
