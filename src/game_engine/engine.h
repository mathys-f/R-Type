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
#include <unordered_map>
#include <vector>

#include "glm/vec2.hpp"
#include "sol/sol.hpp"

#include "assets_manager.h"
#include "ecs/registry.h"
#include "events/event_queue.h"
#include "events/events.h"
#include "events/ui_events.h"
#include "lua_context.h"
#include "snapshots.h"

#define SNAPSHOT_HISTORY_SIZE 96 // 3 secs at 32 tps

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

    // Single event queue for local/client input
    evts::EventQueue<evts::Event> input_event_queue;
    // Per-player event queues for server (indexed by player IP address)
    std::unordered_map<std::string, evts::EventQueue<evts::Event>> player_input_queues;
    evts::EventQueue<evts::UIEvent> ui_event_queue;

    std::unique_ptr<LuaContext> lua_ctx;
    AssetsManager assets_manager;

    const glm::vec2 k_window_size{1080.0f, 720.0f};
    const size_t k_scroll_speed = 5.0f;
    const size_t k_particles = 3;
    const size_t k_stars = 1000;
    const size_t k_max_bullets = 100;
    const size_t k_max_enemies = 8;

    InputContext input_context = InputContext::Gameplay;
    InputState input_state;
    ControlScheme controls = make_default_controls();
    GamepadControlScheme gamepad_controls = make_default_gamepad_controls();
    ControlAction pending_rebind = ControlAction::None;
    GamepadControlAction pending_gamepad_rebind = GamepadControlAction::None;
    bool confirm_keyboard_reset = false;
    bool confirm_gamepad_reset = false;
    bool confirm_enter_rebind = false;
    bool skip_next_gamepad_rebind_input = false;
    unsigned char settings_return_scene = 1;
    int master_volume = 100;
    int music_volume = 100;
    int sfx_volume = 100;
    int last_master_volume = 100;
    int last_music_volume = 100;
    int last_sfx_volume = 100;
    bool master_muted = false;
    bool music_muted = false;
    bool sfx_muted = false;

    std::string server_ip;
    std::uint16_t server_port;
    // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

    void add_scene_loader(unsigned char scene_id, std::function<void(EngineContext&)> loader);
    void set_scene(unsigned char scene_id);
    unsigned char get_current_scene() const;

    std::size_t get_current_tick() const;

    const SnapshotRecord &get_latest_snapshot(std::size_t player_id) const;
    void record_snapshot(SnapshotRecord &snapshot);

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

    std::size_t m_current_tick = 0;

    std::unordered_map<size_t, std::vector<SnapshotRecord>> m_snapshots_history;
};

} // namespace engn

#include "engine.tcc"
