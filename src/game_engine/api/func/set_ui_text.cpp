#include "api/lua.h"
#include "components/ui/ui_text.h"
#include "engine.h"
#include "utils/logger.h"

#include "sol/sol.hpp"

using namespace engn;

namespace {
constexpr int k_default_font_size = 16;
} // namespace

static auto read_text(sol::table table) {
    cpnt::UIText text;
    text.content = table["content"].get_or(std::string{"Text"});
    text.font_size = table["font_size"].get_or(k_default_font_size);
    return text;
}

void lua::set_ui_text(EngineContext& ctx, unsigned char scene_id, std::string tag, sol::table t) {
    if (scene_id != ctx.get_current_scene()) {
        LOG_WARNING("Set_ui_text: Attempted to edit {} in scene {}, but current scene is {}", tag,
                    static_cast<size_t>(scene_id), static_cast<size_t>(ctx.get_current_scene()));
        return;
    }
    auto entity = ctx.registry.get_tag_registry().get_entity(tag);

    if (!entity.has_value()) {
        LOG_WARNING("Set_ui_text: No entity found with tag '{}'", tag);
        return;
    }

    cpnt::Scene scene{scene_id};
    ctx.registry.add_component(entity.value(), std::move(scene));

    cpnt::UIText text = read_text(t);

    ctx.registry.add_component(entity.value(), std::move(text));
}
