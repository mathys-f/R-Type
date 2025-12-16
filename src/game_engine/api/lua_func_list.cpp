#include "api/lua.h"

#include <memory>
#include <vector>
#include <string>

namespace engn::lua {

const std::vector<std::unique_ptr<LuaApiEntryBase>> k_api_functions = []() {
    std::vector<std::unique_ptr<LuaApiEntryBase>> temp;
    temp.emplace_back(std::make_unique<LuaApiEntry<unsigned char, std::string>>("Create_ui_button", create_ui_button));
    temp.emplace_back(std::make_unique<LuaApiEntry<unsigned char, std::string, sol::table>>("Create_ui_text", create_ui_text));
    temp.emplace_back(std::make_unique<LuaApiEntry<unsigned char, std::string, sol::table>>("Set_ui_style", set_ui_style));
    temp.emplace_back(std::make_unique<LuaApiEntry<unsigned char, std::string, sol::table>>("Set_ui_transform", set_ui_transform));
    temp.emplace_back(std::make_unique<LuaApiEntry<unsigned char, std::string, sol::table>>("Set_ui_text", set_ui_text));
    temp.emplace_back(std::make_unique<LuaApiEntry<unsigned char, std::string, sol::table>>("Set_ui_navigation", set_ui_navigation));
    return temp;
}();

} // namespace engn::lua
