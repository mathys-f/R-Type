#include "api/lua.h"

#include <string>

#include "sol/sol.hpp"

#include "engine.h"
#include "components/tag.h"
#include "components/ui/ui_transform.h"
#include "components/ui/ui_text.h"
#include "components/ui/ui_style.h"

#include "utils/logger.h"

using namespace engn;

void lua::create_ui_text(EngineContext &ctx, std::string name, sol::table t) {
    const ecs::Entity e = ctx.registry.spawn_entity();

    ecs::TagRegistry::TagId id = ctx.registry.tag_registry.create_and_bind_tag(name, e);
    if (id == ecs::TagRegistry::k_invalid_tag_id) {
        LOG_WARNING("Create_ui_text: Tag '{}' already exists", name);
        return;
    }

    cpnt::Tag tag{id};
    ctx.registry.add_component(e, std::move(tag));

    cpnt::UIText text;
    ctx.registry.add_component(e, std::move(text));

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
}
