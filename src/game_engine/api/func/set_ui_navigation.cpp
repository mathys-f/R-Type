#include "api/lua.h"

#include "sol/sol.hpp"

#include "engine.h"
#include "utils/logger.h"
#include "components/ui/ui_navigation.h"

using namespace engn;

static auto read_navigation(sol::table table) {
    cpnt::UINavigation navigation;
    navigation.up_tag = table["up"].get_or(ecs::TagRegistry::TagId{0});
    navigation.down_tag = table["down"].get_or(ecs::TagRegistry::TagId{0});
    navigation.left_tag = table["left"].get_or(ecs::TagRegistry::TagId{0});
    navigation.right_tag = table["right"].get_or(ecs::TagRegistry::TagId{0});
    return navigation;
}

void lua::set_ui_navigation(EngineContext &ctx, std::string tag, sol::table t)
{
    auto entity = ctx.registry.tag_registry.get_entity(tag);

    if (!entity.has_value()) {
        LOG_WARNING("Set_ui_navigation: No entity found with tag '{}'", tag);
        return;
    }

    cpnt::UINavigation navigation = read_navigation(t);

    ctx.registry.add_component(entity.value(), std::move(navigation));
}
