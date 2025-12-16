#pragma once

#include "snapshot_meta.h"

namespace engn {

namespace cpnt {

struct Transform {
    float x;
    float y;
    float z;

    float origin_x;
    float origin_y;

    float rx;
    float ry;
    float rz;

    float sx;
    float sy;
    float sz;

    SnapshotMeta snapshot_meta;
};

} // namespace component

} // namespace engine

