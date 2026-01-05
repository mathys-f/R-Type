#pragma once

#include "components/i_sync_component.h"

namespace engn::cpnt {

struct Velocity : ISyncComponent{
    float vx{};
    float vy{};
    float vz{};

    float vrx{};
    float vry{};
    float vrz{};

    Velocity() = default;
    Velocity(float vx, float vy, float vz = 0.0f,
             float vrx = 0.0f, float vry = 0.0f, float vrz = 0.0f);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt