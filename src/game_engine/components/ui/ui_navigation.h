#pragma once

#include "ecs/tag_registry.h"

namespace engn {

namespace cpnt {

struct UINavigation {
    ecs::TagRegistry::TagId up_tag;
    ecs::TagRegistry::TagId down_tag;
    ecs::TagRegistry::TagId left_tag;
    ecs::TagRegistry::TagId right_tag;
};

} // namespace cpnt

} // namespace engn
