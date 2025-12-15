#pragma once
#include "api/lua_api_entry.h"
#include "components/components.h"

namespace engn {

class EngineContext;

namespace lua {

void expose_components(sol::state &lua);
void load_lua_scripts(sol::state &lua, const std::string &scripts_dir);

extern const std::vector<std::unique_ptr<LuaApiEntryBase>> k_api_functions;

void create_ui_button(EngineContext &ctx, std::string name);
void set_ui_transform(EngineContext &ctx, std::string name, sol::table t);
void set_ui_style(EngineContext &ctx, std::string name, sol::table t);

}  // namespace lua

} // namespace engn
