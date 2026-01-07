#pragma once

#include "assets_manager.h"
#include "controls.h"
#include "ecs/registry.h"
#include "events/event_queue.h"
#include "events/events.h"
#include "events/ui_events.h"
#include "lua_context.h"

#include <functional>
#include <memory>
#include <vector>

#include "glm/vec2.hpp"
#include "sol/sol.hpp"

namespace engn {

class EngineContext {
  public:
    EngineContext();
    ~EngineContext() = default;
    EngineContext(const EngineContext&) = delete;
    EngineContext& operator=(const EngineContext&) = delete;
    EngineContext(EngineContext&&) = delete;
    EngineContext& operator=(EngineContext&&) = delete;

    // NOLINTBEGIN(cppcoreguidelines-non-private-member-variables-in-classes)
    float delta_time = 0.0f;
    bool should_quit = false;

    ecs::Registry registry;
    ecs::Entity focused_entity;

    evts::EventQueue<evts::Event> input_event_queue;
    evts::EventQueue<evts::UIEvent> ui_event_queue;

    std::unique_ptr<LuaContext> lua_ctx;
    AssetsManager assets_manager;

    const glm::vec2 k_window_size{1080.0f, 720.0f};
    const size_t k_scroll_speed = 5.0f;
    const size_t k_particles = 3;
    const size_t k_stars = 1000;
    const size_t k_max_bullets = 100;
    const size_t k_max_enemies = 8;

    ControlScheme controls = make_default_controls();
    ControlAction pending_rebind = ControlAction::None;
    unsigned char settings_return_scene = 1;

    std::string server_ip;
    std::uint16_t server_port;
    // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

    void add_scene_loader(unsigned char scene_id, std::function<void(EngineContext&)> loader);
    void set_scene(unsigned char scene_id);
    unsigned char get_current_scene() const;

    // System registration / execution
    /// Register a system that accepts const views to the requested
    /// component storages. The callable should accept `(Registry&, const
    /// SparseArray<...>&...)` for the listed component types.
    /// @tparam TComponents Component types the system requires.
    /// @tparam TFunction Callable type.
    /// @param f The function object to register.
    template <class... TComponents, typename TFunction> void add_system(TFunction&& f);

    /// Register a const-qualified callable system.
    template <class... TComponents, typename TFunction> void add_system(TFunction const& f);

    /// Execute all registered systems in order they have been added.
    void run_systems();

  private:
    unsigned char m_current_scene;
    std::unordered_map<unsigned char, std::function<void(EngineContext&)>> m_scenes_loaders;

    std::vector<std::function<void(EngineContext&)>> m_systems;
};

} // namespace engn

#include "engine.tcc"
