#include "../components/components.h"
#include "api/lua.h"
#include "utils/color.h"
#include "utils/logger.h"

#include "sol/sol.hpp"

using namespace engn;

void lua::expose_components(sol::state& lua) {
    LOG_DEBUG("Exposing components to Lua");

    lua.new_usertype<utils::Color>(
        "Color",
        "r", sol::property([](utils::Color& c) { return c.r; }, [](utils::Color& c, unsigned char v) { c.r = v; }),
        "g", sol::property([](utils::Color& c) { return c.g; }, [](utils::Color& c, unsigned char v) { c.g = v; }),
        "b", sol::property([](utils::Color& c) { return c.b; }, [](utils::Color& c, unsigned char v) { c.b = v; }),
        "a", sol::property([](utils::Color& c) { return c.a; }, [](utils::Color& c, unsigned char v) { c.a = v; }));

    lua.new_usertype<cpnt::UITransform>(
        "UITransform",
        "x", sol::property([](cpnt::UITransform& t) { return t.x; }, [](cpnt::UITransform& t, float v) { t.x = v; }),
        "y", sol::property([](cpnt::UITransform& t) { return t.y; }, [](cpnt::UITransform& t, float v) { t.y = v; }),
        "z", sol::property([](cpnt::UITransform& t) { return t.z; }, [](cpnt::UITransform& t, float v) { t.z = v; }),
        "w", sol::property([](cpnt::UITransform& t) { return t.w; }, [](cpnt::UITransform& t, float v) { t.w = v; }),
        "h", sol::property([](cpnt::UITransform& t) { return t.h; }, [](cpnt::UITransform& t, float v) { t.h = v; }),
        "anchor_x", sol::property([](cpnt::UITransform& t) { return t.anchor_x; }, [](cpnt::UITransform& t, float v) { t.anchor_x = v; }),
        "anchor_y", sol::property([](cpnt::UITransform& t) { return t.anchor_y; }, [](cpnt::UITransform& t, float v) { t.anchor_y = v; }),
        "rotation", sol::property([](cpnt::UITransform& t) { return t.rotation; }, [](cpnt::UITransform& t, float v) { t.rotation = v; }));

    lua.new_usertype<cpnt::UIStyle>(
        "UIStyle",
        "background_color", sol::property([](cpnt::UIStyle& s) { return s.background_color; }, [](cpnt::UIStyle& s, utils::Color v) { s.background_color = v; }),
        "background_color_hovered", sol::property([](cpnt::UIStyle& s) { return s.background_color_hovered; }, [](cpnt::UIStyle& s, utils::Color v) { s.background_color_hovered = v; }),
        "background_color_pressed", sol::property([](cpnt::UIStyle& s) { return s.background_color_pressed; }, [](cpnt::UIStyle& s, utils::Color v) { s.background_color_pressed = v; }),
        "text_color", sol::property([](cpnt::UIStyle& s) { return s.text_color; }, [](cpnt::UIStyle& s, utils::Color v) { s.text_color = v; }),
        "text_color_hovered", sol::property([](cpnt::UIStyle& s) { return s.text_color_hovered; }, [](cpnt::UIStyle& s, utils::Color v) { s.text_color_hovered = v; }),
        "text_color_pressed", sol::property([](cpnt::UIStyle& s) { return s.text_color_pressed; }, [](cpnt::UIStyle& s, utils::Color v) { s.text_color_pressed = v; }),
        "border_color", sol::property([](cpnt::UIStyle& s) { return s.border_color; }, [](cpnt::UIStyle& s, utils::Color v) { s.border_color = v; }),
        "border_color_hovered", sol::property([](cpnt::UIStyle& s) { return s.border_color_hovered; }, [](cpnt::UIStyle& s, utils::Color v) { s.border_color_hovered = v; }),
        "border_color_pressed", sol::property([](cpnt::UIStyle& s) { return s.border_color_pressed; }, [](cpnt::UIStyle& s, utils::Color v) { s.border_color_pressed = v; }),
        "border_thickness", sol::property([](cpnt::UIStyle& s) { return s.border_thickness; }, [](cpnt::UIStyle& s, float v) { s.border_thickness = v; }),
        "border_radius", sol::property([](cpnt::UIStyle& s) { return s.border_radius; }, [](cpnt::UIStyle& s, float v) { s.border_radius = v; }));

    lua.new_usertype<cpnt::UINavigation>(
        "UINavigation",
        "up", sol::property([](cpnt::UINavigation& n) { return n.up_tag; }, [](cpnt::UINavigation& n, ecs::TagRegistry::TagId v) { n.up_tag = v; }),
        "down", sol::property([](cpnt::UINavigation& n) { return n.down_tag; }, [](cpnt::UINavigation& n, ecs::TagRegistry::TagId v) { n.down_tag = v; }),
        "left", sol::property([](cpnt::UINavigation& n) { return n.left_tag; }, [](cpnt::UINavigation& n, ecs::TagRegistry::TagId v) { n.left_tag = v; }),
        "right", sol::property([](cpnt::UINavigation& n) { return n.right_tag; }, [](cpnt::UINavigation& n, ecs::TagRegistry::TagId v) { n.right_tag = v; }));

    lua.new_usertype<cpnt::UIButton>("UIButton");
}
