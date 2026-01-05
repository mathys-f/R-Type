#pragma once

#include <array>
#include <cstddef>

#include "components/i_sync_component.h"

namespace engn::cpnt {

struct Health : ISyncComponent {
    int hp{};
    int max_hp{};
    int changes{};
    
    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
