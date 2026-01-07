#pragma once

#include "snapshot_meta.h"

#include <string>

namespace engn {

namespace cpnt {

struct Replicated {
    uint32_t tag{};

    SnapshotMeta snapshot_meta;
};

} // namespace cpnt

} // namespace engn
