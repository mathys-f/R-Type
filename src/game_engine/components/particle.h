#pragma once

#include "snapshot_meta.h"

namespace engn {

namespace cpnt {

struct Particle {
    float lifetime;
    float maxLifetime;

    SnapshotMeta snapshot_meta;
};

} // namespace cpnt

} // namespace engn
