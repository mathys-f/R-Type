#pragma once

#include <cstdint>

#include "components/i_sync_component.h"

namespace engn::cpnt {

struct BossHitbox : ISyncComponent {
    float width_1{}, height_1{};
    float offset_x_1{}, offset_y_1{};

    float width_2{}, height_2{};
    float offset_x_2{}, offset_y_2{};

    float width_3{}, height_3{};
    float offset_x_3{}, offset_y_3{};


    BossHitbox() = default;
    BossHitbox(float width_1 = 0.0f, float height_1 = 0.0f, float offset_x_1 = 0.0f, float offset_y_1 = 0.0f,
               float width_2 = 0.0f, float height_2 = 0.0f, float offset_x_2 = 0.0f, float offset_y_2 = 0.0f,
               float width_3 = 0.0f, float height_3 = 0.0f, float offset_x_3 = 0.0f, float offset_y_3 = 0.0f);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
