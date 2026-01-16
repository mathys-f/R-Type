#include "api/lua.h"
#include "components/ui/ui_transform.h"
#include "engine.h"
#include "utils/logger.h"

#include "sol/sol.hpp"

using namespace engn;

static auto read_transform(sol::table table) {
    cpnt::UITransform trans{};
    trans.x = table["x"].get_or(0.0f);
    trans.y = table["y"].get_or(0.0f);
    trans.z = table["z"].get_or(0.0f);
    trans.w = table["w"].get_or(100.0f);
    trans.h = table["h"].get_or(100.0f);
    trans.anchor_x = table["anchor_x"].get_or(0.0f);
    trans.anchor_y = table["anchor_y"].get_or(0.0f);
    trans.rotation = table["rotation"].get_or(0.0f);
    return trans;
}

void lua::set_ui_transform(EngineContext& ctx, std::string tag, sol::table t) {
    auto entity = ctx.registry.get_tag_registry().get_entity(tag);

    if (!entity.has_value()) {
        LOG_WARNING("Set_ui_transform: No entity found with tag '{}'", tag);
        return;
    }

    cpnt::UITransform transform = read_transform(t);

    ctx.registry.add_component(entity.value(), std::move(transform));
}
