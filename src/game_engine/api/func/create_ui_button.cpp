#include "api/lua.h"

#include <string>

#include "engine.h"
#include "components/tag.h"
#include "components/ui/ui_transform.h"
#include "components/ui/ui_button.h"
#include "components/ui/ui_focusable.h"
#include "components/ui/ui_navigation.h"
#include "components/ui/ui_style.h"
#include "components/ui/ui_interactable.h"

#include "utils/logger.h"

using namespace engn;

void lua::create_ui_button(EngineContext &ctx, unsigned char scene_id, std::string name) {
    const ecs::Entity e = ctx.registry.spawn_entity();

    ecs::TagRegistry::TagId id = ctx.registry.tag_registry.create_and_bind_tag(name, e);
    if (id == ecs::TagRegistry::k_invalid_tag_id) {
        LOG_WARNING("Create_ui_button: Tag '{}' already exists", name);
        return;
    }

    cpnt::Tag tag{id};
    ctx.registry.add_component(e, std::move(tag));

    cpnt::Scene scene{scene_id};
    ctx.registry.add_component(e, std::move(scene));

    cpnt::UIInteractable interactable{false, false, false};
    ctx.registry.add_component(e, std::move(interactable));

    cpnt::UIFocusable focusable{true};
    ctx.registry.add_component(e, std::move(focusable));

    cpnt::UIButton button;
    ctx.registry.add_component(e, std::move(button));

    cpnt::UITransform transform{0, 0, 1000, 100, 50, 0.0f, 0.0f, 0.0f};
    ctx.registry.add_component(e, std::move(transform));

    cpnt::UIStyle style{
        utils::Color{200, 200, 200, 255},
        utils::Color{220, 220, 220, 255},
        utils::Color{180, 180, 180, 255},
        utils::Color{0, 0, 0, 255},
        utils::Color{0, 0, 0, 255},
        utils::Color{0, 0, 0, 255},
        utils::Color{0, 0, 0, 255},
        utils::Color{30, 30, 30, 255},
        utils::Color{60, 60, 60, 255},
        5.0f,
        2.0f
    };
    ctx.registry.add_component(e, std::move(style));

    cpnt::UINavigation navigation{ecs::TagRegistry::k_invalid_tag_id,
                                  ecs::TagRegistry::k_invalid_tag_id,
                                  ecs::TagRegistry::k_invalid_tag_id,
                                  ecs::TagRegistry::k_invalid_tag_id};
    ctx.registry.add_component(e, std::move(navigation));
}
