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

static auto read_text(sol::table table) {
    cpnt::UIText text;
    text.content = table["content"].get_or(std::string{"Text"});
    text.font_size = table["font_size"].get_or(16);
    return text;
}

void lua::create_ui_text(EngineContext &ctx, unsigned char scene_id, std::string name, sol::table t) {
    if (scene_id != ctx.get_current_scene()) {
        LOG_WARNING("Create_ui_text: Attempted to create Text {} in scene {}, but current scene is {}", name, static_cast<size_t>(scene_id), static_cast<size_t>(ctx.get_current_scene()));
        return;
    }
    const ecs::Entity e = ctx.registry.spawn_entity();

    ecs::TagRegistry::TagId id = ctx.registry.tag_registry.create_and_bind_tag(name, e);
    if (id == ecs::TagRegistry::k_invalid_tag_id) {
        LOG_WARNING("Create_ui_text: Tag '{}' already exists", name);
        return;
    }

    cpnt::Tag tag{id};
    ctx.registry.add_component(e, std::move(tag));

    cpnt::Scene scene{scene_id};
    ctx.registry.add_component(e, std::move(scene));

    cpnt::UIText text = read_text(t);
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
