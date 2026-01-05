#pragma once

#include "components/i_sync_component.h"

namespace engn::cpnt {

struct Explosion : ISyncComponent {
    enum class ExplosionType { Small, Large };

    ExplosionType type{};
    float timer{};
    float frame_duration{};
    int current_frame{};
    int total_frames{};

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
