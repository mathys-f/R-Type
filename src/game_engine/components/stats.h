#pragma once

#include "snapshot_meta.h"

namespace engn {

namespace cpnt {

struct Stats {
    int score;
    int dmg;
    int kills;

    SnapshotMeta snapshot_meta;
};

} // namespace component

} // namespace engine
