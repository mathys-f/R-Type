#pragma once

namespace engn::cpnt {

struct Explosion {
    enum class ExplosionType { Small, Large };

    ExplosionType type{};
    float timer{};
    float frame_duration{};
    int current_frame{};
    int total_frames{};
};

} // namespace engn::cpnt
