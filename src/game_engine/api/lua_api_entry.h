#pragma once

#include <functional>
#include <string>

#include "sol/sol.hpp"

namespace engn {

class EngineContext;

namespace lua {

class LuaApiEntryBase {
 public:
    virtual ~LuaApiEntryBase() = default;
    virtual void expose(sol::state&, EngineContext&) const = 0;
};

template <typename... TArgs>
class LuaApiEntry : public LuaApiEntryBase {
 public:
    LuaApiEntry(std::string name, std::function<void(EngineContext &, TArgs...)> func);

    void expose(sol::state &lua, EngineContext &ctx) const override;
 private:
    std::string m_name;
    std::function<void(EngineContext &, TArgs...)> m_func;
};

}  // namespace lua

} // namespace engn

#include "lua_api_entry.tcc"
