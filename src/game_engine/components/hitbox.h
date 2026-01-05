#pragma once

#include "snapshot_meta.h"

#include <cstdint>

namespace engn {

namespace cpnt {

struct Hitbox {
    float width{}, height{};
    float offset_x{}, offset_y{};

    SnapshotMeta snapshot_meta;
};

} // namespace cpnt

} // namespace engn
