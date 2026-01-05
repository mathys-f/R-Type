#include "game_engine/api/lua.h"
#include "game_engine/engine.h"
#include "game_engine/systems/systems.h"
#include "scenes_loaders.h"
#include "systems/client_systems.h"

#include <string>

#include "sol/sol.hpp"

using namespace engn;

const std::string k_script_file = "scripts/lua/ui/server_login_menu.lua";

void load_server_connection_scene(engn::EngineContext& engine_ctx) {
    auto& reg = engine_ctx.registry;

    reg.register_component<cpnt::UIButton>();
    reg.register_component<cpnt::UIFocusable>();
    reg.register_component<cpnt::UIInputField>();
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
    engine_ctx.add_system<cpnt::UIInteractable>(sys::ui_input_field_updater);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIStyle>(sys::ui_background_renderer);
    engine_ctx.add_system<cpnt::UITransform, cpnt::UIText, cpnt::UIStyle>(sys::ui_text_renderer);
    engine_ctx.add_system<>(handle_connection_menu_ui_events);

    engn::lua::load_lua_script_from_file(engine_ctx.lua_ctx->get_lua_state(), k_script_file);
}
