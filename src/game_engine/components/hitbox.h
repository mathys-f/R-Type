#pragma once

#include <cstdint>

#include "components/i_sync_component.h"

namespace engn::cpnt {

struct Hitbox : ISyncComponent {
    float width{}, height{};
    float offset_x{}, offset_y{};
    
    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
