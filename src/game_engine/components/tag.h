#pragma once

#include "ecs/tag_registry.h"
#include "snapshot_meta.h"

namespace engn {

namespace cpnt {

struct Tag {
    ecs::TagRegistry::TagId id = ecs::TagRegistry::k_invalid_tag_id;

    SnapshotMeta snapshot_meta;
};

} // namespace cpnt

} // namespace engn
