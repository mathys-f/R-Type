#pragma once

#include "engine.h" // Does nothing due to include guards, used for ide auto-completion

namespace engn {

/// Register a system that will be executed later via `run_systems`.
/// The callable is wrapped to fetch const references to the requested
/// component storages and forward them to the provided function.
template <class... Components, typename Function> void EngineContext::add_system(Function&& f)
{
    (void)std::initializer_list<int>{(registry.register_component<Components>(), 0)...};

    auto wrapper = [fn = std::forward<Function>(f)](EngineContext& reg) mutable
    { fn(reg, std::as_const(reg.registry.get_components<Components>())...); };

    m_systems.emplace_back(std::move(wrapper));
}

/// Same as the rvalue overload but accepts a const-qualified callable.
template <class... Components, typename Function> void EngineContext::add_system(Function const& f)
{
    (void)std::initializer_list<int>{(registry.register_component<Components>(), 0)...};

    auto wrapper = [&f](EngineContext& reg)
    { f(reg, std::as_const(reg.registry.get_components<Components>())...); };

    m_systems.emplace_back(wrapper);
}

} // namespace engine
