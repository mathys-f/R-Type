#include "api/lua.h"

#include "engine.h"
#include "utils/logger.h"

using namespace engn;

void lua::set_ui_style(EngineContext &ctx, std::string tag, cpnt::UIStyle style)
{
    auto entity = ctx.registry.tag_registry.get_entity(tag);

    if (!entity.has_value()) {
        LOG_WARNING("Set_ui_style: No entity found with tag '{}'", tag);
        return;
    }
    ctx.registry.add_component(entity.value(), std::move(style));
}
