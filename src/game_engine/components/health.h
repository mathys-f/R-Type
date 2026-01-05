#pragma once

#include "snapshot_meta.h"

#include <array>
#include <cstddef>

namespace engn {

namespace cpnt {

struct Health {
    int hp{};
    int max_hp{};
    int changes{};

    SnapshotMeta snapshot_meta;
};

} // namespace cpnt

} // namespace engn
