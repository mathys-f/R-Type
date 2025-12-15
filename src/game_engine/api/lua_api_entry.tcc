#pragma once

#include "api/lua_api_entry.h"

namespace engn::lua {

template <typename... TArgs>
LuaApiEntry<TArgs...>::LuaApiEntry(std::string name, std::function<void(EngineContext &, TArgs...)> func)
    : m_name(name), m_func(func)
{
}

template <typename... TArgs>
void LuaApiEntry<TArgs...>::expose(sol::state &lua, EngineContext &ctx) const
{
    auto &f = this->m_func;
    lua.set_function(m_name, [&f, &ctx](TArgs&&... args) {
        return f(ctx, std::forward<TArgs>(args)...);
    });
}

} // namespace engn::lua
