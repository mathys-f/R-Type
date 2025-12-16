#pragma once

#include <vector>
#include <string>

#include "sol/sol.hpp"


namespace engn {

class LuaContext {
 public:
    LuaContext();
    ~LuaContext() = default;

    sol::state& get_lua_state();

 private:
    sol::state m_lua_state;
};

} // engn
