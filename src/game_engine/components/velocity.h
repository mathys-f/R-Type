#pragma once

#include "snapshot_meta.h"

namespace engn {

namespace cpnt {

struct Velocity {
    float vx{};
    float vy{};
    float vz{};

    float vrx{};
    float vry{};
    float vrz{};

    SnapshotMeta snapshot_meta;
};

} // namespace cpnt

} // namespace engn