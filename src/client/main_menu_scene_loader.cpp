#include "scenes_loaders.h"

#include <string>

#include "sol/sol.hpp"

#include "game_engine/engine.h"
#include "game_engine/api/lua.h"
#include "game_engine/systems/systems.h"
#include "systems/client_systems.h"

using namespace engn;

const std::string k_script_file = "scripts/lua/ui/main_menu.lua";

static float randf() {
    return (rand() % 1000) / 1000.0f;
}

void load_main_menu_scene(engn::EngineContext& engine_ctx)
{
    auto &reg = engine_ctx.registry;

    reg.register_component<cpnt::UIButton>();
    reg.register_component<cpnt::UIFocusable>();
    reg.register_component<cpnt::UIInteractable>();
    reg.register_component<cpnt::UINavigation>();
    reg.register_component<cpnt::UISlider>();
    reg.register_component<cpnt::UIStyle>();
    reg.register_component<cpnt::UIText>();
    reg.register_component<cpnt::UITransform>();

    engine_ctx.add_system<>(sys::fetch_inputs);
    // add_system<>(sys::log_inputs);
    engine_ctx.add_system<cpnt::UITransform>(sys::ui_hover);
    engine_ctx.add_system<>(sys::ui_press);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIStyle>(sys::ui_background_renderer);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIText, cpnt::UIStyle>(sys::ui_text_renderer);
    engine_ctx.add_system<>(handle_main_menu_ui_events);

    reg.register_component<cpnt::Star>();
    engine_ctx.add_system<cpnt::Transform, cpnt::Star>(sys::star_scroll_system);
    engine_ctx.add_system<cpnt::Transform, cpnt::Sprite, cpnt::Star, cpnt::Velocity, cpnt::Particle>(sys::render_system);


    const int k_width = engine_ctx.window_size.x;
    const int k_height = engine_ctx.window_size.y;

    for (int i = 0; i < engine_ctx.stars; i++) {
        auto star = engine_ctx.registry.spawn_entity();
        engine_ctx.registry.add_component(star, engn::cpnt::Transform{
            (float)GetRandomValue(0, k_width),
            (float)GetRandomValue(0, k_height),
            0, 0, 0, 0, 1, 1, 1
        });
        engine_ctx.registry.add_component(star, cpnt::Star{randf()});
    }

    engn::lua::load_lua_script_from_file(engine_ctx.lua_ctx->get_lua_state(), k_script_file);
}
