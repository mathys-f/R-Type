#include "engine.h"

#include "api/lua.h"
#include "components/components.h"
#include "systems/systems.h"
#include "utils/logger.h"

#include <filesystem>

using namespace engn;

namespace {
constexpr unsigned char k_max_scene_id = 255;
} // namespace

static void expose_cpp_api(sol::state& lua, EngineContext& ctx);

EngineContext::EngineContext() : server_port(0), m_current_scene(0) {
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
}

static void expose_cpp_api(sol::state& lua, EngineContext& ctx) {
    LOG_DEBUG("Exposing C++ API to Lua");
    for (const auto& entry : engn::lua::k_api_functions) {
        entry->expose(lua, ctx);
    }
}

void engn::EngineContext::add_client(std::size_t client_id, asio::ip::udp::endpoint client_endpoint) {
    std::lock_guard<std::mutex> lock(m_clients_mutex);
    m_clients[client_id] = client_endpoint;
}

void engn::EngineContext::remove_client(asio::ip::udp::endpoint client_endpoint) {
    std::lock_guard<std::mutex> lock(m_clients_mutex);
    for (auto it = m_clients.begin(); it != m_clients.end(); ++it) {
        if (it->second == client_endpoint) {
            m_clients.erase(it);
            break;
        }
    }
}

const std::unordered_map<std::size_t, asio::ip::udp::endpoint> &engn::EngineContext::get_clients() const {
    std::lock_guard<std::mutex> lock(m_clients_mutex);
    return m_clients;
}

void EngineContext::add_scene_loader(unsigned char scene_id, std::function<void(EngineContext&)> loader) {
    m_scenes_loaders[scene_id] = loader;
}

void EngineContext::set_scene(unsigned char scene_id) {
    if (scene_id > k_max_scene_id) {
        LOG_ERROR("Max number of scenes is 256 (0-255)");
        return;
    } else if (scene_id == m_current_scene) {
        LOG_INFO("Reloading scene {}", static_cast<int>(scene_id));
    } else if (m_scenes_loaders[scene_id] == nullptr) {
        LOG_ERROR("Scene {} has no loader function registered", static_cast<int>(scene_id));
        return;
    }
    LOG_INFO("Switching to scene {}", static_cast<int>(scene_id));
    LOG_DEBUG("Clearing registry...");
    registry.~Registry();
    new (&registry) ecs::Registry();
    m_systems.clear();
    m_current_scene = scene_id;
    LOG_DEBUG("Spawning initial entity {}",
              static_cast<std::size_t>(registry.spawn_entity())); // ensure entity 0 is reserved
    LOG_DEBUG("Loading scene {}...", static_cast<int>(scene_id));
    m_scenes_loaders[scene_id](*this);
    LOG_INFO("Scene {} loaded", static_cast<int>(scene_id));
}

unsigned char EngineContext::get_current_scene() const {
    return m_current_scene;
}

std::size_t engn::EngineContext::get_current_tick() const {
    return m_current_tick;
}

const SnapshotRecord& engn::EngineContext::get_latest_snapshot() const {
    static SnapshotRecord s_empty_record; // Need to be static to return reference

    if (m_snapshots_history.empty())
       return s_empty_record;

    const auto &history = m_snapshots_history.begin()->second;

    const SnapshotRecord &record = history[m_current_tick % SNAPSHOT_HISTORY_SIZE];
    if (!record.snapshot.entities.empty())
        return record;
    return s_empty_record;
}

const SnapshotRecord& engn::EngineContext::get_latest_acknowledged_snapshot(std::size_t player_id) const {
    static SnapshotRecord s_empty_record; // Need to be static to return reference

    if (m_snapshots_history.find(player_id) == m_snapshots_history.end())
       return s_empty_record;

    const auto &history = m_snapshots_history.at(player_id);

    for (std::size_t tick = m_current_tick; tick > 0; tick--) {
        const SnapshotRecord &record = history[tick % SNAPSHOT_HISTORY_SIZE];

        if (record.acknowledged)
            return record;
    }
    return s_empty_record;
}

void engn::EngineContext::record_snapshot(SnapshotRecord &record) {
    for (auto &history : m_snapshots_history)
        std::get<1>(history)[m_current_tick % SNAPSHOT_HISTORY_SIZE] = record;
}
