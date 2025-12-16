#pragma once

#include <string>
#include "snapshot_meta.h"

namespace engn {

namespace cpnt {

struct Replicated {
    std::string tag;

    SnapshotMeta snapshot_meta;
};

} // namespace cpnt

} // namespace engn
