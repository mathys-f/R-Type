#include "lua_context.h"

#include <filesystem>
#include <iostream>

using namespace engn;

LuaContext::LuaContext()
{
    m_lua_state.open_libraries(sol::lib::base);
}

sol::state &LuaContext::getLuaState()
{
    return m_lua_state;
}
