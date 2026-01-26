#include "systems/systems.h"

#include <mutex>

#include "ecs/zipper.h"
#include "game_engine/engine.h"

using namespace engn;

/// Create & deletes player entities based on connected clients
/// Also manages player ID assignments
void sys::server_update_player_entities_system(EngineContext &ctx,
    ecs::SparseArray<cpnt::Player> const& players) {
    std::lock_guard<std::mutex> lock_a(ctx.player_input_queues_mutex);
    auto &reg = ctx.registry;
    auto clients = ctx.get_clients();

    // Create new player for each client who doesn't have one yet
    for (const auto &client : clients) {
        std::uint8_t player_id = 0;
        bool found_id = false;

        // Reuse existing id if the client is already mapped
        for (const auto& [id, endpoint] : ctx.player_id_to_endpoint) {
            if (endpoint == client) {
                player_id = id;
                found_id = true;
                break;
            }
        }

        // Otherwise find the next available player ID
        if (!found_id) {
            for (std::uint8_t id = 0; id < ctx.k_player_count; ++id) {
                if (ctx.player_id_to_endpoint.find(id) == ctx.player_id_to_endpoint.end()) {
                    player_id = id;
                    found_id = true;
                    ctx.player_id_to_endpoint[player_id] = client;
                    break;
                }
            }
        }

        if (!found_id) {
            continue; // No available IDs
        }

        if (ctx.player_input_queues.find(client) == ctx.player_input_queues.end()) {
            ctx.player_input_queues[client] = evts::EventQueue<evts::Event>{};
        }

        if (ctx.dead_player_ids.find(player_id) != ctx.dead_player_ids.end()) {
            continue;
        }

        // Skip if the player entity already exists for this id
        bool already_spawned = false;
        for (const auto& [entity_id, player_opt] : ecs::indexed_zipper(players)) {
            if (player_opt && player_opt->id == player_id) {
                already_spawned = true;
                break;
            }
        }

        if (already_spawned) {
            continue;
        }

        // Create player's entity
        constexpr float k_ship_sprite_x = 166.0f;
        constexpr float k_ship_sprite_y = 0.0f;
        constexpr float k_ship_width = 33.0f;
        constexpr float k_ship_height = 18.0f;
        constexpr float k_ship_scale = 3.0f;

        // NOLINTBEGIN(cppcoreguidelines-narrowing-conversions, cppcoreguidelines-pro-type-union-access)
        const int k_width = static_cast<int>(ctx.window_size.x);
        const int k_height = static_cast<int>(ctx.window_size.y);
        // NOLINTEND(cppcoreguidelines-narrowing-conversions, cppcoreguidelines-pro-type-union-access)

        Rectangle ship_source_rect = {k_ship_sprite_x, k_ship_sprite_y, k_ship_width, k_ship_height};


        auto player = ctx.registry.spawn_entity();
        ctx.registry.add_component(
            player, cpnt::Transform{(float)k_width / 2, (float)k_height / 2, 0, 0, 0, 0, 0, 0, 1, 1, 1});
        cpnt::Player player_cpnt;
        player_cpnt.id = player_id;
        ctx.registry.add_component(player, std::move(player_cpnt));
        ctx.registry.add_component(player, cpnt::Health{ctx.k_player_health, ctx.k_player_health});
        ctx.registry.add_component(player, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
        ctx.registry.add_component(player,
            cpnt::Hitbox{ k_ship_width  * k_ship_scale / 2,
            k_ship_height * k_ship_scale / 2,
            ship_source_rect.height / 3,
            ship_source_rect.width / 3}
        );
        ctx.registry.add_component(player, cpnt::Replicated{static_cast<std::uint32_t>(player)});
        ctx.registry.add_component(player, cpnt::EntityType{"player"});
    }



    // Remove player entities for disconnected clients
    std::vector<std::uint8_t> disconnected_player_ids;
    for (const auto& [player_id, endpoint] : ctx.player_id_to_endpoint)
        if (std::find(clients.begin(), clients.end(), endpoint) == clients.end())
            disconnected_player_ids.push_back(player_id);

    for (const auto& player_id : disconnected_player_ids) {
        auto endpoint = ctx.player_id_to_endpoint[player_id];
        ctx.player_id_to_endpoint.erase(player_id);
        ctx.player_input_queues.erase(endpoint);
        ctx.dead_player_ids.erase(player_id);

        // Remove the player's entity
        for (auto [idx, player_opt] : ecs::indexed_zipper(players)) {
            if (player_opt && player_opt->id == player_id) {
                auto entity = reg.entity_from_index(idx);
                reg.kill_entity_deferred(entity);
                break;
            }
        }
    }

}
