#include "api/lua.h"

#include "sol/sol.hpp"

#include "engine.h"
#include "utils/logger.h"

using namespace engn;

static auto read_color(sol::table table) {
    utils::Color color;
    color.r = table["r"].get_or(0);
    color.g = table["g"].get_or(0);
    color.b = table["b"].get_or(0);
    color.a = table["a"].get_or(255);
    return color;
}

void lua::set_ui_style(EngineContext &ctx, unsigned char scene_id, std::string tag, sol::table t)
{
    if (scene_id != ctx.get_current_scene()) {
        LOG_WARNING("Set_ui_style: Attempted to edit {} in scene {}, but current scene is {}", tag, static_cast<size_t>(scene_id), static_cast<size_t>(ctx.get_current_scene()));
        return;
    }
    auto entity = ctx.registry.tag_registry.get_entity(tag);

    if (!entity.has_value()) {
        LOG_WARNING("Set_ui_style: No entity found with tag '{}'", tag);
        return;
    }

    cpnt::Scene scene{scene_id};
    ctx.registry.add_component(entity.value(), std::move(scene));

    cpnt::UIStyle style;

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
