#pragma once

#include "snapshot_meta.h"

namespace engn {

namespace cpnt {

struct Explosion {
    enum class ExplosionType { Small, Large };

    ExplosionType type{};
    float timer{};
    float frame_duration{};
    int current_frame{};
    int total_frames{};

    SnapshotMeta snapshot_meta;
};

} // namespace cpnt

} // namespace engn
