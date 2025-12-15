#pragma once

#include <string>
#include <variant>

#include "ecs/tag_registry.h"

namespace engn {

namespace evts {

struct UIButtonClicked {
    ecs::TagRegistry::TagId tag;
};

struct UISliderChanged {
    ecs::TagRegistry::TagId tag;
    float new_value;
};

struct UICheckboxToggled {
    ecs::TagRegistry::TagId tag;
    bool is_checked;
};

struct UIInputSubmit {
    ecs::TagRegistry::TagId tag;
    std::string text;
};

using UIEvent = std::variant<
    UIButtonClicked,
    UISliderChanged,
    UICheckboxToggled,
    UIInputSubmit
>;

} // namespace evts

} // namespace engn
