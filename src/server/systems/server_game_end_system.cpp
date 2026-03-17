#include "systems/systems.h"
#include "engine.h"

#include "ecs/zipper.h"

#include <cstring>

using namespace engn;

constexpr int k_boss_kill_to_win = 2;

static net::Packet create_end_packet(const cpnt::Stats& stats, int boss_kill_to_win);

void sys::server_end_game_system(EngineContext &ctx,
    ecs::SparseArray<cpnt::Player> const &players,
    ecs::SparseArray<cpnt::Stats> const &stats,
    ecs::SparseArray<cpnt::Health> const &healths)
{
    static std::unordered_map<asio::ip::udp::endpoint, std::uint32_t> s_packets_sent;
    auto k_clients = ctx.get_clients();
    const cpnt::Stats* run_stats = nullptr;

    for (const auto &[stat] : ecs::zipper(stats)) {
        if (stat.has_value()) {
            run_stats = &stat.value();
            break;
        }
    }

    if (run_stats == nullptr) {
        return;
    }

    if (k_clients.empty()) {
        if (s_packets_sent.empty()) {
            return; // Waiting for clients
        } else {
            // No clients connected but endgame has already been attained once
            LOG_INFO("No clients connected but endgame msg has already been sent, stopping lobby");
            ctx.should_quit = true;
            return;
        }
    }

    if (players.size() == 0) return; // Game not rally started yet

    const bool k_boss_goal_reached = run_stats->boss_killed >= k_boss_kill_to_win;
    bool all_players_dead = true;

    for (const auto &[id, player, health] : ecs::indexed_zipper(players, healths)) {
        if (player.has_value() && health.has_value()) {
            if (health->hp > 0) {
                all_players_dead = false;
                break;
            }
        }
    }

    if (!all_players_dead && !k_boss_goal_reached) {
        return;
    }

    if (s_packets_sent.empty()) {
        // Clients connected and an endgame condition has been reached.
        for (auto &endpoint : k_clients) {
            net::Packet paquet = create_end_packet(*run_stats, k_boss_kill_to_win);

            uint32_t id = ctx.network_session->send(paquet, endpoint, true);
            // Needs to be reliable to know when to stop server
            s_packets_sent[endpoint] = id;
        }
    } else if (!s_packets_sent.empty()) {
        // Game end packet already sent, waiting for answer

        for (auto &[endpoint, id] : s_packets_sent) {
            if (std::find(k_clients.begin(), k_clients.end(), endpoint) == k_clients.end()) {
                continue; // Don't wait for clients already gone
            }

            net::DeliveryStatus status = ctx.network_session->is_message_acknowledged(id, endpoint);

            if (status == net::DeliveryStatus::Pending) {
                return; // Don't stop the server if a client is not aware of the endgame info
            }

            if (status == net::DeliveryStatus::TimedOut || status == net::DeliveryStatus::Failed) {
                LOG_WARNING("Failed to send final results to a client ({}), retrying",
                    (status == net::DeliveryStatus::TimedOut ? "Timed Out" : "Failed"));

                net::Packet paquet = create_end_packet(*run_stats, k_boss_kill_to_win);
                uint32_t id = ctx.network_session->send(paquet, endpoint, true);
                // Needs to be reliable to know when to stop server
                s_packets_sent[endpoint] = id;

                return; // Don't stop the server if a client is not aware of the endgame info
            }
        }
        // Right now, the code checked that all clients have received the endgame info.
        LOG_INFO("All clients have received endgame msg, shutting down lobby.");
        ctx.should_quit = true; // Time to stop the server !
    }
}

static net::Packet create_end_packet(const cpnt::Stats& stats, int boss_kill_to_win)
{
    net::Packet paquet;
    const SerializedComponent k_serialized_stats = stats.serialize();
    const std::size_t k_payload_size = k_serialized_stats.data.size() + sizeof(boss_kill_to_win);

    paquet.header.m_command = static_cast<std::uint8_t>(net::CommandId::kGameEnded);
    paquet.header.m_payload_size = static_cast<std::uint16_t>(k_payload_size);
    paquet.payload = k_serialized_stats.data;
    paquet.payload.resize(k_payload_size);

    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    std::memcpy(paquet.payload.data() + k_serialized_stats.data.size(), &boss_kill_to_win, sizeof(boss_kill_to_win));
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    return paquet;
}
