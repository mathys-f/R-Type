#include "api/lua.h"

#include <memory>
#include <vector>
#include <string>

namespace engn::lua {

const std::vector<std::unique_ptr<LuaApiEntryBase>> k_api_functions = []() {
    std::vector<std::unique_ptr<LuaApiEntryBase>> temp;
    temp.emplace_back(std::make_unique<LuaApiEntry<std::string>>("Create_ui_button", create_ui_button));
    temp.emplace_back(std::make_unique<LuaApiEntry<std::string, sol::table>>("Set_ui_style", set_ui_style));
    return temp;
}();

} // namespace engn::lua
