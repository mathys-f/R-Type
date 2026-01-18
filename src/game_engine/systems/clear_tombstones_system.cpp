#include "systems/systems.h"

#include "engine.h"

using namespace engn;

/// This system will remove thumbstones that all of the clients are now aware of
/// Also clears the metadatas of components additions/modifications when the same condition is satisfied
void sys::clear_tombstones_system(EngineContext &ctx)
{
    if (ctx.get_clients().empty()) return; // We should keep everythinf that has happened if nobody's here for now

    std::vector<ecs::Registry::EntityType> creation_tombstones_to_delete;
    std::vector<ecs::Registry::EntityType> remove_tombstones_to_delete;
    std::unordered_multimap<ecs::Registry::EntityType, std::type_index> component_destruction_tombstones_to_delete;
    std::unordered_multimap<ecs::Registry::EntityType, std::type_index> component_metadatas_to_delete;

    for (const auto &[id, version] : ctx.registry.get_entity_creation_tombstones()) {
        bool should_delete = true;

        for (const auto &client : ctx.get_clients()) {
            std::lock_guard<std::mutex> lock_a(ctx.clients_mutex);
            if (ctx.get_latest_acknowledged_snapshot(client).last_update_tick < version)
                should_delete = false;
        }

        if (should_delete)
            creation_tombstones_to_delete.push_back(id);
    }

    for (const auto &[id, version] : ctx.registry.get_entity_destruction_tombstones()) {
        bool should_delete = true;

        for (const auto &client : ctx.get_clients()) {
            std::lock_guard<std::mutex> lock_a(ctx.clients_mutex);
            if (ctx.get_latest_acknowledged_snapshot(client).last_update_tick < version)
                should_delete = false;
        }

        if (should_delete)
            remove_tombstones_to_delete.push_back(id);
    }

    for (const auto &[id, cpnt_list] : ctx.registry.get_component_destruction_tombstones()) {
        for (const auto &[type_idx, version] : cpnt_list) {
            bool should_delete = true;

            for (const auto &client : ctx.get_clients()) {
                std::lock_guard<std::mutex> lock_a(ctx.clients_mutex);
                if (ctx.get_latest_acknowledged_snapshot(client).last_update_tick < version)
                    should_delete = false;
            }

            if (should_delete)
                component_destruction_tombstones_to_delete.emplace(id, type_idx);
        }
    }

    for (const auto &[cpnt, version] : ctx.registry.get_component_metadata()) {
        bool should_delete = true;

        for (const auto &client : ctx.get_clients()) {
            std::lock_guard<std::mutex> lock_a(ctx.clients_mutex);
            if (ctx.get_latest_acknowledged_snapshot(client).last_update_tick < version)
                should_delete = false;
        }

        if (should_delete)
            component_metadatas_to_delete.emplace(std::get<0>(cpnt), std::get<1>(cpnt));
    }

    // Delete the marked tombstones and metadata
    for (const auto &id : creation_tombstones_to_delete)
        ctx.registry.remove_entity_creation_tombstone(id);

    for (const auto &id : remove_tombstones_to_delete)
        ctx.registry.remove_entity_destruction_tombstone(id);

    for (const auto &[id, type_idx] : component_destruction_tombstones_to_delete)
        ctx.registry.remove_component_destruction_tombstone(id, type_idx);

    for (const auto &[entity, type_idx] : component_metadatas_to_delete)
        ctx.registry.remove_component_metadata(entity, type_idx);
}
