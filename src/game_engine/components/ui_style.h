#pragma once

#include "utils/color.h"

namespace engn {

namespace cpnt {

// Style component used for UI elements.
// Stores colors(bg, border, text), border radius & padding.
struct UIStyle {
    utils::Color background_color;
    utils::Color text_color;
    utils::Color border_color;

    float border_radius;
    float padding;
};

} // namespace components

} // namespace engine
