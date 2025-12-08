#pragma once

#include "ecs/tag_registry.h"

namespace engn {

namespace cpnt {

struct Tag {
    ecs::TagRegistry::TagId id = ecs::TagRegistry::k_invalid_tag_id;
};

} // namespace cpnt

} // namespace engn
