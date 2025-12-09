#pragma once

#include "utils/color.h"

namespace engn {

namespace cpnt {

// Style component used for UI elements.
struct UIStyle {
    utils::Color background_color;
    utils::Color background_color_hovered;
    utils::Color background_color_pressed;
    utils::Color text_color;
    utils::Color text_color_hovered;
    utils::Color text_color_pressed;
    utils::Color border_color;
    utils::Color border_color_hovered;
    utils::Color border_color_pressed;

    float border_radius;
    float border_thickness;
    float padding;
};

} // namespace components

} // namespace engine
