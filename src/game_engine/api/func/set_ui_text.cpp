#include "api/lua.h"

#include "sol/sol.hpp"

#include "engine.h"
#include "utils/logger.h"
#include "components/ui/ui_text.h"

using namespace engn;

static auto read_text(sol::table table) {
    cpnt::UIText text;
    text.content = table["content"].get_or(std::string{"Text"});
    text.font_size = table["font_size"].get_or(16);
    return text;
}

void lua::set_ui_text(EngineContext &ctx, std::string tag, sol::table t)
{
    auto entity = ctx.registry.tag_registry.get_entity(tag);

    if (!entity.has_value()) {
        LOG_WARNING("Set_ui_text: No entity found with tag '{}'", tag);
        return;
    }

    cpnt::UIText text = read_text(t);

    ctx.registry.add_component(entity.value(), std::move(text));
}
