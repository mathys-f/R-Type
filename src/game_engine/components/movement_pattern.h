#pragma once

#include "snapshot_meta.h"

namespace engn {

namespace cpnt {

struct MovementPattern {
    enum class PatternType { Straight, Sine, ZigZag, Dive };

    PatternType type{};
    float speed{};
    float amplitude{};
    float frequency{};
    float timer{};
    float base_y{};
    SnapshotMeta snapshot_meta;
};

} // namespace cpnt

} // namespace engn
