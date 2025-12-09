#pragma once

#include <vector>
#include <functional>
#include <memory>

#include "glm/vec2.hpp"
#include "sol/sol.hpp"

#include "ecs/registry.h"
#include "events/events.h"
#include "events/ui_events.h"
#include "events/event_queue.h"
#include "lua_context.h"

namespace engn {

class EngineContext {
 public:
    EngineContext(bool headless);
    ~EngineContext() = default;

    ecs::Registry registry;

    evts::EventQueue<evts::Event> input_event_queue;
    evts::EventQueue<evts::UIEvent> ui_event_queue;

    ecs::Entity focused_entity;

    std::unique_ptr<LuaContext> lua_ctx;

    float delta_time = 0.0f;

    const glm::vec2 window_size{1080.0f, 720.0f};

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
