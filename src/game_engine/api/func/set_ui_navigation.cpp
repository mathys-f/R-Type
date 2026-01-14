#include "api/lua.h"
#include "components/scene.h"
#include "components/ui/ui_navigation.h"
#include "engine.h"
#include "utils/logger.h"

#include "sol/sol.hpp"

using namespace engn;

static ecs::TagRegistry::TagId read_tag_id(const ecs::TagRegistry& tags, const sol::table& table, const char* key) {
    const sol::object k_value = table[key];
    if (!k_value.valid())
        return ecs::TagRegistry::k_invalid_tag_id;
    if (k_value.is<ecs::TagRegistry::TagId>())
        return k_value.as<ecs::TagRegistry::TagId>();
    if (k_value.is<std::string>())
        return tags.get_tag_id(k_value.as<std::string>());
    return ecs::TagRegistry::k_invalid_tag_id;
}

static auto read_navigation(const ecs::TagRegistry& tags, sol::table table) {
    cpnt::UINavigation navigation{};
    navigation.up_tag = read_tag_id(tags, table, "up");
    navigation.down_tag = read_tag_id(tags, table, "down");
    navigation.left_tag = read_tag_id(tags, table, "left");
    navigation.right_tag = read_tag_id(tags, table, "right");
    return navigation;
}

void lua::set_ui_navigation(EngineContext& ctx, unsigned char scene_id, std::string tag, sol::table t) {
    if (scene_id != ctx.get_current_scene()) {
        LOG_WARNING("Set_ui_navigation: Attempted to edit {} in scene {}, but current scene is {}", tag,
                    static_cast<size_t>(scene_id), static_cast<size_t>(ctx.get_current_scene()));
        return;
    }
    auto entity = ctx.registry.get_tag_registry().get_entity(tag);

    if (!entity.has_value()) {
        LOG_WARNING("Set_ui_navigation: No entity found with tag '{}'", tag);
        return;
    }

    cpnt::Scene scene{scene_id};
    ctx.registry.add_component(entity.value(), std::move(scene));

    cpnt::UINavigation navigation = read_navigation(ctx.registry.get_tag_registry(), t);

    ctx.registry.add_component(entity.value(), std::move(navigation));
}
