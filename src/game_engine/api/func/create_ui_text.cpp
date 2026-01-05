#include "api/lua.h"
#include "components/tag.h"
#include "components/ui/ui_style.h"
#include "components/ui/ui_text.h"
#include "components/ui/ui_transform.h"
#include "engine.h"
#include "utils/logger.h"

#include <string>

#include "sol/sol.hpp"

using namespace engn;

constexpr int k_default_font_size = 16;
constexpr int k_text_x = 0;
constexpr int k_text_y = 0;
constexpr int k_text_width = 1000;
constexpr int k_text_height = 100;
constexpr int k_text_padding = 50;

constexpr unsigned char k_alpha_opaque = 255;
constexpr unsigned char k_color_light_gray = 200;
constexpr unsigned char k_color_lighter_gray = 220;
constexpr unsigned char k_color_dark_gray = 180;
constexpr unsigned char k_color_very_dark_gray = 30;
constexpr unsigned char k_color_darker_gray = 60;

constexpr float k_text_border_radius = 5.0f;
constexpr float k_text_border_width = 2.0f;

static auto read_text(sol::table table) {
    cpnt::UIText text;
    text.content = table["content"].get_or(std::string{"Text"});
    text.font_size = table["font_size"].get_or(k_default_font_size);
    return text;
}

void lua::create_ui_text(EngineContext& ctx, unsigned char scene_id, std::string name, sol::table t) {
    if (scene_id != ctx.get_current_scene()) {
        LOG_WARNING("Create_ui_text: Attempted to create Text {} in scene {}, but current scene is {}", name,
                    static_cast<size_t>(scene_id), static_cast<size_t>(ctx.get_current_scene()));
        return;
    }
    const ecs::Entity k_e = ctx.registry.spawn_entity();

    ecs::TagRegistry::TagId id = ctx.registry.get_tag_registry().create_and_bind_tag(name, k_e);
    if (id == ecs::TagRegistry::k_invalid_tag_id) {
        LOG_WARNING("Create_ui_text: Tag '{}' already exists", name);
        return;
    }

    cpnt::Tag tag{id};
    ctx.registry.add_component(k_e, std::move(tag));

    cpnt::Scene scene{scene_id};
    ctx.registry.add_component(k_e, std::move(scene));

    cpnt::UIText text = read_text(t);
    ctx.registry.add_component(k_e, std::move(text));

    cpnt::UITransform transform{k_text_x, k_text_y, k_text_width, k_text_height, k_text_padding, 0.0f, 0.0f, 0.0f};
    ctx.registry.add_component(k_e, std::move(transform));

    cpnt::UIStyle style{
        utils::Color{k_color_light_gray, k_color_light_gray, k_color_light_gray, k_alpha_opaque},
        utils::Color{k_color_lighter_gray, k_color_lighter_gray, k_color_lighter_gray, k_alpha_opaque},
        utils::Color{k_color_dark_gray, k_color_dark_gray, k_color_dark_gray, k_alpha_opaque},
        utils::Color{0, 0, 0, k_alpha_opaque},
        utils::Color{0, 0, 0, k_alpha_opaque},
        utils::Color{0, 0, 0, k_alpha_opaque},
        utils::Color{0, 0, 0, k_alpha_opaque},
        utils::Color{k_color_very_dark_gray, k_color_very_dark_gray, k_color_very_dark_gray, k_alpha_opaque},
        utils::Color{k_color_darker_gray, k_color_darker_gray, k_color_darker_gray, k_alpha_opaque},
        k_text_border_radius,
        k_text_border_width};
    ctx.registry.add_component(k_e, std::move(style));
}
