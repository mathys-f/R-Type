#pragma once

#include <vector>
#include <functional>

#include "ecs/registry.h"
#include "events/events.h"
#include "events/ui_events.h"

namespace engn {

class EngineContext {
 public:
    EngineContext() = default;
    ~EngineContext() = default;

    ecs::Registry ecs;

    std::vector<evts::Event> input_events;
    std::vector<evts::UIEvent> ui_events;

    float delta_time = 0.0f;

    // System registration / execution
    /// Register a system that accepts const views to the requested
    /// component storages. The callable should accept `(Registry&, const
    /// SparseArray<...>&...)` for the listed component types.
    /// @tparam Components Component types the system requires.
    /// @tparam Function Callable type.
    /// @param f The function object to register.
    template <class... Components, typename Function> void add_system(Function&& f);

    /// Register a const-qualified callable system.
    template <class... Components, typename Function> void add_system(Function const& f);

    /// Execute all registered systems in order they have been added.
    void run_systems();

 private:
    std::vector<std::function<void(EngineContext&)>> m_systems;

};

} // namespace engine

#include "engine.tcc"
