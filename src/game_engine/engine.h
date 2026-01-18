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
#include <mutex>
#include <unordered_map>

#include "glm/vec2.hpp"
#include "sol/sol.hpp"

#include "networking/rtp/networking.h"

#include "assets_manager.h"
#include "ecs/registry.h"
#include "events/event_queue.h"
#include "events/events.h"
#include "events/ui_events.h"
#include "lua_context.h"
#include "snapshots.h"
#include "network_client.h"

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

    evts::EventQueue<evts::Event> input_event_queue;
    evts::EventQueue<evts::UIEvent> ui_event_queue;

    std::unique_ptr<LuaContext> lua_ctx;
    AssetsManager assets_manager;

    glm::vec2 window_size{1080.0f, 720.0f};
    glm::vec2 k_sim_size{1920.0f, 1080.0f};

    const size_t k_max_bullets = 100;

    // Graphics settings (modifiable at runtime via pause menu)
    size_t k_scroll_speed = 5;
    size_t k_particles = 3;
    size_t k_stars = 1000;

    // Difficulty settings (adjustable in main menu)
    size_t k_max_charger = 5;
    size_t k_max_shooter = 2;
    float k_shooter_base_speed = 3.0f;
    float k_shooter_speed_variance = 5.0f;
    int k_shooter_health = 3;
    int k_spawn_margin = 100;
    float k_enemy_base_speed = 3.0f;
    float k_enemy_speed_variance = 5.0f;
    int k_enemy_health = 3;
    float k_pattern_speed_variance = 3.0f;
    int k_pattern_amplitude_max = 10;
    int k_player_health = 100;

    std::shared_ptr<net::Session> network_session; // TO REMOVE
    std::shared_ptr<NetworkClient> network_client; // High level network client wrapper around the session

    const std::size_t k_player_count = 4;
    std::mutex clients_mutex;
    void add_client(asio::ip::udp::endpoint client_endpoint);
    void remove_client(asio::ip::udp::endpoint client_endpoint);
    // Mutex 'clients_mutex' must be locked when using
    const std::vector<asio::ip::udp::endpoint> &get_clients();

    std::mutex snapshots_history_mutex;
    void record_snapshot(SnapshotRecord &snapshot);
    // Mutex 'snapshots_history_mutex' must be locked when using
    SnapshotRecord &get_latest_snapshot(asio::ip::udp::endpoint endpoint);
    // Mutex 'snapshots_history_mutex' must be locked when using
    const SnapshotRecord& get_latest_acknowledged_snapshot(asio::ip::udp::endpoint endpoint);

    std::unordered_map<asio::ip::udp::endpoint, std::vector<SnapshotRecord>>& get_snapshots_history();

    void add_snapshot_delta(WorldDelta &delta);
    /// After being run, will send back ACKs to the server and clear the deltas list
    void for_each_snapshot_delta(std::function<void(EngineContext &ctx, const WorldDelta&)> func);

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
    std::string settings_return_scene = "";
    int master_volume = 100;
    int music_volume = 100;
    int sfx_volume = 100;
    int last_master_volume = 100;
    int last_music_volume = 100;
    int last_sfx_volume = 100;
    bool master_muted = false;
    bool music_muted = false;
    bool sfx_muted = false;

    bool change_music = false;

    std::string server_ip;
    std::uint16_t server_port;

    class BackendAPIClient* backend_api_client = nullptr; // Backend sync (server-side only)
    std::uint32_t current_lobby_id = 0; // Current lobby ID for backend sync
    // NOLINTEND(cppcoreguidelines-non-private-member-variables-in-classes)

    void add_scene_loader(const std::string &scene_name, std::function<void(EngineContext&)> loader);
    void set_scene(const std::string &scene_name);
    const std::string &get_current_scene() const;

    std::size_t get_current_tick() const;

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
    std::string m_current_scene;
    std::unordered_map<std::string, std::function<void(EngineContext&)>> m_scenes_loaders;

    std::vector<std::function<void(EngineContext&)>> m_systems;

    std::size_t m_current_tick = 1; // 0 is reserved for error values

    std::unordered_map<asio::ip::udp::endpoint, std::vector<SnapshotRecord>> m_snapshots_history;

    std::mutex m_snapshots_delta_mutex;
    std::vector<WorldDelta> m_snapshots_delta;

    // Mutex 'clients_mutex' in public
    std::vector<asio::ip::udp::endpoint> m_clients;
};

} // namespace engn

#include "engine.tcc"
