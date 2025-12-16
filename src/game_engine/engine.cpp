#include "engine.h"

#include <filesystem>

#include "components/components.h"
#include "systems/systems.h"
#include "api/lua.h"
#include "utils/logger.h"

using namespace engn;

const std::string k_scripts_dir = "scripts/";

static void expose_cpp_api(sol::state &lua, EngineContext &ctx);

EngineContext::EngineContext(bool headless)
{
    if (!headless) {
        lua_ctx = std::make_unique<LuaContext>();

        add_system<>(sys::fetch_inputs);
        // add_system<>(sys::log_inputs);
        add_system<cpnt::UITransform>(sys::ui_hover);
        add_system<>(sys::ui_press);
        add_system<cpnt::UITransform, cpnt::UIStyle>(sys::ui_background_renderer);
        add_system<cpnt::UITransform, cpnt::UIText, cpnt::UIStyle>(sys::ui_text_renderer);
        add_system<>(sys::manage_ui_events);

        lua::expose_components(lua_ctx->getLuaState());
        expose_cpp_api(lua_ctx->getLuaState(), *this);
        lua::load_lua_scripts(lua_ctx->getLuaState(), k_scripts_dir);
    }

    registry.spawn_entity(); // ensure entity 0 is reserved
}

void EngineContext::run_systems()
{
    input_event_queue.clear();
    ui_event_queue.clear();
    for (auto& sys : m_systems) {
        sys(*this);
    }
}

static void expose_cpp_api(sol::state &lua, EngineContext &ctx) {
    LOG_DEBUG("Exposing C++ API to Lua");
    for (const auto &entry : engn::lua::k_api_functions) {
        entry->expose(lua, ctx);
    }
}
