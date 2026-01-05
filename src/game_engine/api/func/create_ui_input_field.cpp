#include "api/lua.h"
#include "components/tag.h"
#include "components/ui/ui_input_field.h"
#include "components/ui/ui_interactable.h"
#include "components/ui/ui_focusable.h"
#include "components/ui/ui_style.h"
#include "components/ui/ui_text.h"
#include "components/ui/ui_transform.h"
#include "engine.h"
#include "utils/logger.h"

using namespace engn;

constexpr float k_input_field_width = 200.0f;
constexpr float k_input_field_height = 30.0f;
constexpr int k_input_field_font_size = 16;

void lua::create_ui_input_field(EngineContext& ctx, unsigned char scene_id, std::string name,
                                std::string default_value) {
    auto& reg = ctx.registry;
    static float s_i = 0;

    LOG_DEBUG("Creating UI Input Field: {}", name);
    ecs::Entity entity = reg.spawn_entity();

    auto tag_id = reg.get_tag_registry().create_and_bind_tag(name, entity);
    cpnt::Tag tag;
    tag.id = tag_id;
    reg.add_component<cpnt::Tag>(entity, std::move(tag));

    reg.add_component<cpnt::UITransform>(entity, cpnt::UITransform{.x = 0.0f,
                                                                   .y = 0.0f,
                                                                   .z = 0.0f,
                                                                   .w = k_input_field_width,
                                                                   .h = k_input_field_height,
                                                                   .anchor_x = 0.0f,
                                                                   .anchor_y = 0.0f,
                                                                   .rotation = 0.0f});

    reg.add_component<cpnt::UIStyle>(entity, cpnt::UIStyle{});

    reg.add_component<cpnt::UIFocusable>(entity, cpnt::UIFocusable{.is_focused = false});

    reg.add_component<cpnt::UIInteractable>(
        entity, cpnt::UIInteractable{.hovered = false, .pressed = false, .released = false});

    reg.add_component<cpnt::UIInputField>(entity, cpnt::UIInputField{.editing = false, .timer = s_i++});

    reg.add_component<cpnt::UIText>(entity,
                                    cpnt::UIText{.content = default_value, .font_size = k_input_field_font_size});
}
