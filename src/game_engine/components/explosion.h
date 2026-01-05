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

    Explosion() = default;
    Explosion(ExplosionType type, float frame_duration, int total_frames, 
              float timer = 0.0f, int current_frame = 0);

    engn::SerializedComponent serialize() const override;
    void deserialize(const std::vector<std::byte>& data) override;
};

} // namespace engn::cpnt
