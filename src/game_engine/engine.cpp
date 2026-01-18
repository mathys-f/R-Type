#include "engine.h"

#include "api/lua.h"
#include "components/components.h"
#include "utils/logger.h"

#include <filesystem>

using namespace engn;

namespace {
constexpr unsigned char k_max_scene_id = 255;
} // namespace

static void expose_cpp_api(sol::state& lua, EngineContext& ctx);

EngineContext::EngineContext() : server_port(0), m_current_scene("") {
    m_snapshots_history.reserve(4); // Reserve for 4 players
    for (auto &snapshot: m_snapshots_history)
        snapshot.second.reserve(SNAPSHOT_HISTORY_SIZE); // Make sure enough space for snapshots hystory
    lua_ctx = std::make_unique<LuaContext>();
    lua::expose_components(lua_ctx->get_lua_state());
    expose_cpp_api(lua_ctx->get_lua_state(), *this);
}

void EngineContext::run_systems() {
    input_event_queue.clear();
    ui_event_queue.clear();
    auto curr_scene_buff = m_current_scene;
    for (auto& sys : m_systems) {
        if (curr_scene_buff != m_current_scene) {
            LOG_DEBUG("Scene changed during system execution, stopping further system execution");
            break;
        }
        if (sys)
            sys(*this);
    }
    m_current_tick++;
    registry.set_current_version(m_current_tick);
}

static void expose_cpp_api(sol::state& lua, EngineContext& ctx) {
    LOG_DEBUG("Exposing C++ API to Lua");
    for (const auto& entry : engn::lua::k_api_functions) {
        entry->expose(lua, ctx);
    }
}

void EngineContext::add_client(asio::ip::udp::endpoint client_endpoint) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    std::lock_guard<std::mutex> lock_b(snapshots_history_mutex);
    m_clients.push_back(client_endpoint);
    m_snapshots_history[client_endpoint] = std::vector<SnapshotRecord>(SNAPSHOT_HISTORY_SIZE);
}

void EngineContext::remove_client(asio::ip::udp::endpoint client_endpoint) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    auto it = std::find(m_clients.begin(), m_clients.end(), client_endpoint);
    if (it != m_clients.end()) {
        m_clients.erase(it);
    }
}

const std::vector<asio::ip::udp::endpoint> &EngineContext::get_clients() {
    std::lock_guard<std::mutex> lock(clients_mutex);
    return m_clients;
}

void EngineContext::add_scene_loader(const std::string &scene_name, std::function<void(EngineContext&)> loader) {
    m_scenes_loaders[scene_name] = loader;
}

void EngineContext::set_scene(const std::string &scene_name) {
    if (scene_name == m_current_scene) {
        LOG_INFO("Reloading scene {}", scene_name);
    } else if (m_scenes_loaders[scene_name] == nullptr) {
        LOG_ERROR("Scene {} has no loader function registered", scene_name);
        return;
    }
    LOG_INFO("Switching to scene {}", scene_name);

    // Disconnect network client if it exists before scene change
    if (network_client && network_client->is_connected()) {
        LOG_DEBUG("Disconnecting network client...");
        network_client->disconnect();
        network_client.reset();
    }

    LOG_DEBUG("Clearing registry...");
    registry.~Registry();
    new (&registry) ecs::Registry();
    m_systems.clear();
    LOG_DEBUG("Spawning initial entity {}",
              static_cast<std::size_t>(registry.spawn_entity())); // ensure entity 0 is reserved
    LOG_DEBUG("Loading scene {}...", scene_name);
    m_scenes_loaders[scene_name](*this);
    if ((scene_name == "main_menu") ||
        (m_current_scene == "main_menu" && scene_name == "singleplayer_game")) {
        change_music = true;
    }
    m_current_scene = scene_name;
    LOG_INFO("Scene {} loaded", scene_name);
}

const std::string &EngineContext::get_current_scene() const {
    return m_current_scene;
}

std::size_t EngineContext::get_current_tick() const {
    return m_current_tick;
}

SnapshotRecord& EngineContext::get_latest_snapshot(asio::ip::udp::endpoint endpoint) {
    std::lock_guard<std::mutex> lock(snapshots_history_mutex);
    static SnapshotRecord s_empty_record; // Need to be static to return reference

    if (m_snapshots_history.empty()) {
       return s_empty_record;
    }

    auto &history = m_snapshots_history.at(endpoint);

    SnapshotRecord &record = history[m_current_tick % SNAPSHOT_HISTORY_SIZE];
    if (!record.snapshot.entities.empty()) {
        return record;
    }
    return s_empty_record;
}

const SnapshotRecord& EngineContext::get_latest_acknowledged_snapshot(asio::ip::udp::endpoint endpoint) {
    std::lock_guard<std::mutex> lock(snapshots_history_mutex);
    static SnapshotRecord s_empty_record; // Need to be static to return reference

    s_empty_record.acknowledged = true; // The empty record is sent if the player has never acknowledged anything yet
    s_empty_record.last_update_tick = 0;
    if (m_snapshots_history.find(endpoint) == m_snapshots_history.end())
       return s_empty_record;

    const auto &history = m_snapshots_history.at(endpoint);

    for (std::size_t tick = m_current_tick; tick > 0; tick--) {
        const SnapshotRecord &record = history[tick % SNAPSHOT_HISTORY_SIZE];

        if (record.acknowledged) {
            return record;
        }
    }
    return s_empty_record;
}

void EngineContext::record_snapshot(SnapshotRecord &record) {
    std::lock_guard<std::mutex> lock(snapshots_history_mutex);
    record.last_update_tick = m_current_tick;
    for (auto &history : m_snapshots_history)
        std::get<1>(history)[m_current_tick % SNAPSHOT_HISTORY_SIZE] = record;
}

std::unordered_map<asio::ip::udp::endpoint, std::vector<SnapshotRecord>>& engn::EngineContext::get_snapshots_history() {
    std::lock_guard<std::mutex> lock(snapshots_history_mutex);
    return m_snapshots_history;
}

void EngineContext::add_snapshot_delta(WorldDelta &delta) {
    std::lock_guard<std::mutex> lock(m_snapshots_delta_mutex);
    m_snapshots_delta.push_back(delta);
}

void EngineContext::for_each_snapshot_delta(std::function<void(EngineContext &ctx, const WorldDelta&)> func) {
    std::lock_guard<std::mutex> lock(m_snapshots_delta_mutex);
    for (const auto &delta : m_snapshots_delta) {
        func(*this, delta);
    }
    m_snapshots_delta.clear();
}
