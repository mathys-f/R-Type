#include "api/lua.h"

#include "sol/sol.hpp"

#include "../components/components.h"
#include "utils/logger.h"
#include "utils/color.h"

using namespace engn;

void lua::expose_components(sol::state &lua) {
    LOG_DEBUG("Exposing components to Lua");

    lua.new_usertype<utils::Color>("Color",
        "r", &utils::Color::r,
        "g", &utils::Color::g,
        "b", &utils::Color::b,
        "a", &utils::Color::a
    );

    lua.new_usertype<cpnt::UITransform>("UITransform",
        "x", &cpnt::UITransform::x,
        "y", &cpnt::UITransform::y,
        "z", &cpnt::UITransform::z,
        "w", &cpnt::UITransform::w,
        "h", &cpnt::UITransform::h,
        "anchor_x", &cpnt::UITransform::anchor_x,
        "anchor_y", &cpnt::UITransform::anchor_y,
        "rotation", &cpnt::UITransform::rotation
    );

    lua.new_usertype<cpnt::UIStyle>("UIStyle",
        "background_color", &cpnt::UIStyle::background_color,
        "background_color_hovered", &cpnt::UIStyle::background_color_hovered,
        "background_color_pressed", &cpnt::UIStyle::background_color_pressed,
        "text_color", &cpnt::UIStyle::text_color,
        "text_color_hovered", &cpnt::UIStyle::text_color_hovered,
        "text_color_pressed", &cpnt::UIStyle::text_color_pressed,
        "border_color", &cpnt::UIStyle::border_color,
        "border_color_hovered", &cpnt::UIStyle::border_color_hovered,
        "border_color_pressed", &cpnt::UIStyle::border_color_pressed,
        "border_thickness", &cpnt::UIStyle::border_thickness,
        "border_radius", &cpnt::UIStyle::border_radius,
        "border_thickness", &cpnt::UIStyle::border_thickness
    );

    lua.new_usertype<cpnt::UINavigation>("UINavigation",
        "up", &cpnt::UINavigation::up_tag,
        "down", &cpnt::UINavigation::down_tag,
        "left", &cpnt::UINavigation::left_tag,
        "right", &cpnt::UINavigation::right_tag
    );

    lua.new_usertype<cpnt::UIButton>("UIButton");
}
