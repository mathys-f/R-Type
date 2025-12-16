#pragma once

#include "snapshot_meta.h"

namespace engn {

namespace cpnt {

struct Explosion {
    enum class ExplosionType {
        Small,
        Large
    };

    ExplosionType type;
    float timer;
    float frameDuration;
    int currentFrame;
    int totalFrames;

    SnapshotMeta snapshot_meta;
};

} // namespace cpnt

} // namespace engn
