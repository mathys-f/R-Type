#pragma once

#include <cstdint>
#include "snapshot_meta.h"

namespace engn {

namespace cpnt {

struct Hitbox {
    float width, height;
    float offsetX, offsetY;

    SnapshotMeta snapshot_meta;
};

} // namespace cpnt

} // namespace engn
