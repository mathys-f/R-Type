#pragma once

#include "components/i_sync_component.h"

namespace engn::cpnt {

struct Transform : ISyncComponent {
    float x{};
    float y{};
    float z{};

    float origin_x{};
    float origin_y{};

    float rx{};
    float ry{};
    float rz{};

    float sx{};
    float sy{};
    float sz{};
    
    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
