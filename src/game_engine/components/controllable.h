#pragma once

#include "snapshot_meta.h"

namespace engn {

namespace cpnt {

struct Controllable {
    float speed;

    SnapshotMeta snapshot_meta;
};

} // namespace cpnt

} // namespace engn