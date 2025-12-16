#pragma once

#include <string>
#include "snapshot_meta.h"

namespace engn {

namespace cpnt {

struct Replicated {
    u_int32_t tag;

    SnapshotMeta snapshot_meta;
};

} // namespace cpnt

} // namespace engn
