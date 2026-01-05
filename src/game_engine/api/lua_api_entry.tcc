#pragma once

#include "lua_api_entry.h"

#include <functional>
#include <string>
#include <utility>

#include "sol/sol.hpp"

namespace engn::lua {

template <typename... TArgs>
LuaApiEntry<TArgs...>::LuaApiEntry(std::string name, std::function<void(EngineContext&, TArgs...)> func)
    : m_name(name), m_func(func) {}

template <typename... TArgs> void LuaApiEntry<TArgs...>::expose(sol::state& lua, EngineContext& ctx) const {
    auto& f = this->m_func;
    lua.set_function(m_name, [&f, &ctx](TArgs... args) { return f(ctx, std::move(args)...); });
}

} // namespace engn::lua
