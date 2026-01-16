#include "api/lua.h"
#include "engine.h"
#include "utils/logger.h"
#include "components/ui/ui_style.h"

#include "sol/sol.hpp"

using namespace engn;

namespace {
constexpr int k_alpha_opaque = 255;
} // namespace

static auto read_color(sol::table table) {
    utils::Color color{};
    color.r = table["r"].get_or(0);
    color.g = table["g"].get_or(0);
    color.b = table["b"].get_or(0);
    color.a = table["a"].get_or(k_alpha_opaque);
    return color;
}

void lua::set_ui_style(EngineContext& ctx, std::string tag, sol::table t) {
    auto entity = ctx.registry.get_tag_registry().get_entity(tag);

    if (!entity.has_value()) {
        LOG_WARNING("Set_ui_style: No entity found with tag '{}'", tag);
        return;
    }

    cpnt::UIStyle style{};

    style.background_color = read_color(t["background_color"]);
    style.background_color_hovered = read_color(t["background_color_hovered"]);
    style.background_color_pressed = read_color(t["background_color_pressed"]);

    style.text_color = read_color(t["text_color"]);
    style.text_color_hovered = read_color(t["text_color_hovered"]);
    style.text_color_pressed = read_color(t["text_color_pressed"]);

    style.border_color = read_color(t["border_color"]);
    style.border_color_hovered = read_color(t["border_color_hovered"]);
    style.border_color_pressed = read_color(t["border_color_pressed"]);

    style.border_radius = t["border_radius"].get_or(0.0f);
    style.border_thickness = t["border_thickness"].get_or(0.0f);

    ctx.registry.add_component(entity.value(), std::move(style));
}
