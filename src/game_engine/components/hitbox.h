#pragma once

#include "components/i_sync_component.h"

#include <cstdint>

namespace engn::cpnt {

struct Hitbox : ISyncComponent {
    float width{}, height{};
    float offset_x{}, offset_y{};

    Hitbox() = default;
    Hitbox(float width, float height, float offset_x = 0.0f, float offset_y = 0.0f);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
