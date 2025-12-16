#include "api/lua.h"

#include "components/components.h"
#include "engine.h"
#include "utils/logger.h"

using namespace engn;

void lua::create_ui_input_field(EngineContext &ctx, unsigned char scene_id,
    std::string name, std::string default_value)
{
    auto &reg = ctx.registry;
    static float i = 0;

    LOG_DEBUG("Creating UI Input Field: {}", name);
    ecs::Entity entity = reg.spawn_entity();

    auto tag_id = reg.tag_registry.create_and_bind_tag(name, entity);
    reg.add_component<cpnt::Tag>(entity, cpnt::Tag{tag_id});

    reg.add_component<cpnt::UITransform>(entity, cpnt::UITransform{
        .x=0.0f,
        .y=0.0f,
        .z=0.0f,
        .w=200.0f,
        .h=30.0f,
        .anchor_x=0.0f,
        .anchor_y=0.0f,
        .rotation=0.0f
    });

    reg.add_component<cpnt::UIStyle>(entity, cpnt::UIStyle{});

    reg.add_component<cpnt::UIFocusable>(entity, cpnt::UIFocusable{
        .is_focused=false
    });

    reg.add_component<cpnt::UIInteractable>(entity, cpnt::UIInteractable{
        .hovered=false,
        .pressed=false,
        .released=false
    });

    reg.add_component<cpnt::UIInputField>(entity, cpnt::UIInputField{
        .editing=false,
        .timer=i++
    });

    reg.add_component<cpnt::UIText>(entity, cpnt::UIText{
        .content=default_value,
        .font_size=16
    });
}
