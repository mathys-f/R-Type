#pragma once

#include <string>
#include <vector>

#include "sol/sol.hpp"

namespace engn {

class LuaContext {
  public:
    LuaContext();
    ~LuaContext() = default;

    LuaContext(const LuaContext&) = delete;
    LuaContext& operator=(const LuaContext&) = delete;
    LuaContext(LuaContext&&) = delete;
    LuaContext& operator=(LuaContext&&) = delete;

    sol::state& get_lua_state();

  private:
    sol::state m_lua_state;
};

} // namespace engn
