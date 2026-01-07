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

    Transform() = default;
    Transform(float x, float y, float z = 0.0f, 
              float origin_x = 0.0f, float origin_y = 0.0f,
              float rx = 0.0f, float ry = 0.0f, float rz = 0.0f,
              float sx = 1.0f, float sy = 1.0f, float sz = 1.0f);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
