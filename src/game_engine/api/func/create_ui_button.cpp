#include "api/lua.h"

#include <string>

#include "engine.h"
#include "components/tag.h"
#include "components/ui/ui_transform.h"
#include "components/ui/ui_button.h"
#include "components/ui/ui_focusable.h"
#include "components/ui/ui_navigation.h"

#include "utils/logger.h"

using namespace engn;

void lua::create_ui_button(EngineContext &ctx, std::string name) {
    const ecs::Entity e = ctx.registry.spawn_entity();

    ecs::TagRegistry::TagId id = ctx.registry.tag_registry.create_and_bind_tag(name, e);
    if (id == ecs::TagRegistry::k_invalid_tag_id) return;

    cpnt::Tag tag{id};
    ctx.registry.add_component(e, std::move(tag));

    cpnt::UIFocusable focusable{true};
    ctx.registry.add_component(e, std::move(focusable));

    cpnt::UIButton button{[&](){
        LOG_WARNING("No callback assigned to the button ! {}", name);
    }};
    ctx.registry.add_component(e, std::move(button));

    cpnt::UITransform transform{0, 0, 1000, 100, 50, 0.0f, 0.0f, 0.0f};
    ctx.registry.add_component(e, std::move(transform));

    cpnt::UINavigation navigation{ecs::TagRegistry::k_invalid_tag_id,
                                  ecs::TagRegistry::k_invalid_tag_id,
                                  ecs::TagRegistry::k_invalid_tag_id,
                                  ecs::TagRegistry::k_invalid_tag_id};
    ctx.registry.add_component(e, std::move(navigation));
}
