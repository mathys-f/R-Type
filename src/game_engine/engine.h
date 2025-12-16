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
#include "assets_manager.h"

namespace engn {

class EngineContext {
 public:
    EngineContext(bool headless);
    ~EngineContext() = default;

    float delta_time = 0.0f;

    ecs::Registry registry;
    ecs::Entity focused_entity;

    evts::EventQueue<evts::Event> input_event_queue;
    evts::EventQueue<evts::UIEvent> ui_event_queue;

    std::unique_ptr<LuaContext> lua_ctx;
    AssetsManager assets_manager;

    const glm::vec2 window_size{1920.0f, 1000.0f};
    const size_t scroll_speed = 5.0f;
    const size_t particles = 3;
    const size_t stars = 1000;
    const size_t max_bullets = 100;
    const size_t max_enemies = 8;

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
