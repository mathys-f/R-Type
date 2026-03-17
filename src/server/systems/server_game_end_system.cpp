#include "systems/systems.h"
#include "engine.h"

#include "ecs/zipper.h"

using namespace engn;

static net::Packet create_end_packet(ecs::SparseArray<cpnt::Stats> const &stats);

void sys::server_end_game_system(EngineContext &ctx,
    ecs::SparseArray<cpnt::Player> const &players,
    ecs::SparseArray<cpnt::Stats> const &stats,
    ecs::SparseArray<cpnt::Health> const &healths)
{
    static std::unordered_map<asio::ip::udp::endpoint, std::uint32_t> s_packets_sent;
    auto k_clients = ctx.get_clients();
    std::lock_guard<std::mutex> lock(ctx.clients_mutex);

    if (k_clients.empty()) return; // Waiting for clients
    if (players.size() == 0) return; // Game not rally started yet


    for (const auto &[id, player, health] : ecs::indexed_zipper(players, healths)) {
        if (player.has_value() && health.has_value()) {
            if (health->hp > 0) {
                LOG_DEBUG("A player is still alive");
                return;
            }
        }
    }

    if (s_packets_sent.empty()) {
        LOG_INFO("Clients connected but no players left (everyone is dead)");
        // Clients connected but no players left (everyone is dead)
        for (auto &endpoint : k_clients) {
            net::Packet paquet = create_end_packet(stats);

            uint32_t id = ctx.network_session->send(paquet, endpoint, true);
            // Needs to be reliable to know when to stop server
            s_packets_sent[endpoint] = id;
        }
    } else if (!s_packets_sent.empty()) {
        LOG_INFO("Game end packet already sent, waiting for answer");
        // Game end packet already sent, waiting for answer

        for (auto &[endpoint, id] : s_packets_sent) {
            if (std::find(k_clients.begin(), k_clients.end(), endpoint) == k_clients.end()) {
                continue; // Don't wait for clients already gone
            }

            net::DeliveryStatus status = ctx.network_session->is_message_acknowledged(id, endpoint);

            if (status == net::DeliveryStatus::Pending) {
                LOG_DEBUG("Waiting for answer from at least one client");
                return; // Don't stop the server if a client is not aware of the endgame info
            }

            if (status == net::DeliveryStatus::TimedOut || status == net::DeliveryStatus::Failed) {
                LOG_WARNING("Failed to send final results to a client ({}), retrying",
                    (status == net::DeliveryStatus::TimedOut ? "Timed Out" : "Failed"));

                net::Packet paquet = create_end_packet(stats);
                uint32_t id = ctx.network_session->send(paquet, endpoint, true);
                // Needs to be reliable to know when to stop server
                s_packets_sent[endpoint] = id;

                return; // Don't stop the server if a client is not aware of the endgame info
            }
        }
        // Right now, the code checked that all clients have received the endgame info.
        LOG_INFO("Right now, the code checked that all clients have received the endgame info.");
        ctx.should_quit = true; // Time to stop the server !
    }
}

static net::Packet create_end_packet(ecs::SparseArray<cpnt::Stats> const &stats)
{
    net::Packet paquet;
    int score = 0;

    paquet.header.m_command = static_cast<std::uint8_t>(net::CommandId::kGameEnded);

    for (const auto &[stat] : ecs::zipper(stats)) {
        if (stat.has_value()) {
            score = stat->score;
        }
    }

    if (score == 0) {
        LOG_WARNING("Could not get endgame score, defaulting to 0: Stats component not found");
    }

    paquet.header.m_payload_size = sizeof(int);
    auto *score_bytes = reinterpret_cast<std::byte *>(&score); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
    paquet.payload = std::vector<std::byte>(score_bytes, score_bytes + sizeof(int)); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)

    return paquet;
}
