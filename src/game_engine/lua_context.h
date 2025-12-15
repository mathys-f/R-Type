#pragma once

#include <vector>
#include <string>

#include "sol/sol.hpp"


namespace engn {

class LuaContext {
 public:
    LuaContext();
    ~LuaContext() = default;

    sol::state& getLuaState();

 private:
    sol::state m_lua_state;
};

} // engn
