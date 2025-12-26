#pragma once

#include "snapshot_meta.h"

namespace engn::cpnt {

struct Scene {
    unsigned char id; // 256 max scenes should be more than enough

    SnapshotMeta snapshot_meta;
};

} // namespace engn::cpnt
