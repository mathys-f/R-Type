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
    
    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt