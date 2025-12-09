#include "lua_context.h"

#include <filesystem>
#include <iostream>

#include "utils/logger.h"
#include "components/components.h"
#include "lua/lua_functions.h"

using namespace engn;

const std::string_view k_scripts_dir = "scripts/";

static void load_lua_scripts(sol::state& lua);

LuaContext::LuaContext()
{
    m_lua_state.open_libraries(sol::lib::base);
    load_lua_scripts(m_lua_state);
}

sol::state &LuaContext::getLuaState()
{
    return m_lua_state;
}

static void load_lua_scripts(sol::state &lua)
{
    std::vector<std::string> scripts_paths;
    int scripts_loaded = 0;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(k_scripts_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".lua") {
            scripts_paths.push_back(entry.path().string());
        }
    }
    LOG_INFO("Loading {} lua scripts from {}", scripts_paths.size(), k_scripts_dir);

    for (const std::string &entry : scripts_paths) {
        try {
            lua.script_file(entry);
            LOG_DEBUG("Loaded lua script from {}", entry);
            scripts_loaded++;
        } catch (const sol::error& e) {
            LOG_ERROR("Failed to load lua script from {}: {}", entry, e.what());
        }
    }

    LOG_INFO("Succesfully loaded {}/{} lua scripts", scripts_loaded, scripts_paths.size());
}
