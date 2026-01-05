#include "api/lua.h"
#include "components/tag.h"
#include "components/scene.h"
#include "components/ui/ui_button.h"
#include "components/ui/ui_focusable.h"
#include "components/ui/ui_interactable.h"
#include "components/ui/ui_navigation.h"
#include "components/ui/ui_style.h"
#include "components/ui/ui_transform.h"
#include "engine.h"
#include "utils/logger.h"

#include <string>

using namespace engn;

constexpr int k_button_x = 0;
constexpr int k_button_y = 0;
constexpr int k_button_width = 1000;
constexpr int k_button_height = 100;
constexpr int k_button_padding = 50;

constexpr unsigned char k_alpha_opaque = 255;
constexpr unsigned char k_color_light_gray = 200;
constexpr unsigned char k_color_lighter_gray = 220;
constexpr unsigned char k_color_dark_gray = 180;
constexpr unsigned char k_color_very_dark_gray = 30;
constexpr unsigned char k_color_darker_gray = 60;

constexpr float k_button_border_radius = 5.0f;
constexpr float k_button_border_width = 2.0f;

void lua::create_ui_button(EngineContext& ctx, unsigned char scene_id, std::string name) {
    if (scene_id != ctx.get_current_scene()) {
        LOG_WARNING("Create_ui_button: Attempted to create button {} in scene {}, but current scene is {}", name,
                    static_cast<size_t>(scene_id), static_cast<size_t>(ctx.get_current_scene()));
        return;
    }
    const ecs::Entity k_e = ctx.registry.spawn_entity();

    ecs::TagRegistry::TagId id = ctx.registry.get_tag_registry().create_and_bind_tag(name, k_e);
    if (id == ecs::TagRegistry::k_invalid_tag_id) {
        LOG_WARNING("Create_ui_button: Tag '{}' already exists", name);
        return;
    }

    cpnt::Tag tag;
    tag.id = id;
    ctx.registry.add_component(k_e, std::move(tag));

    cpnt::Scene scene{scene_id};
    ctx.registry.add_component(k_e, std::move(scene));

    cpnt::UIInteractable interactable{false, false, false};
    ctx.registry.add_component(k_e, std::move(interactable));

    cpnt::UIFocusable focusable{true};
    ctx.registry.add_component(k_e, std::move(focusable));

    cpnt::UIButton button;
    ctx.registry.add_component(k_e, std::move(button));

    cpnt::UITransform transform{k_button_x,       k_button_y, k_button_width, k_button_height,
                                k_button_padding, 0.0f,       0.0f,           0.0f};
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
        k_button_border_radius,
        k_button_border_width};
    ctx.registry.add_component(k_e, std::move(style));

    cpnt::UINavigation navigation{ecs::TagRegistry::k_invalid_tag_id, ecs::TagRegistry::k_invalid_tag_id,
                                  ecs::TagRegistry::k_invalid_tag_id, ecs::TagRegistry::k_invalid_tag_id};
    ctx.registry.add_component(k_e, std::move(navigation));
}
