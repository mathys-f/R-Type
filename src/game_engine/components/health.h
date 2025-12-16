#pragma once

#include <array>
#include <cstddef>
#include "snapshot_meta.h"

namespace engn {

namespace cpnt {

struct Health {
    int hp;
    int max_hp;
    int changes;

    SnapshotMeta snapshot_meta;
};

} // namespace component

} // namespace engine
