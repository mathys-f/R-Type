#pragma once

#include "components/i_sync_component.h"

namespace engn::cpnt {

struct Enemy : ISyncComponent {
    // Tag component for enemies
    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
