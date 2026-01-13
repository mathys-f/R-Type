#include "systems/systems.h"

#include <memory>

#include "networking/rtp/networking.h"

#include "engine.h"

using namespace engn;

static std::optional<WorldDelta> compute_delta(WorldSnapshot const& current,
    ecs::Registry::Version latest_ack_version,
    const ecs::Registry &registry)
{
    WorldDelta delta_snapshot;
    ecs::Registry::Version current_version = registry.get_current_version();

    // New entities
    for (const auto &[id, version] : registry.get_entity_creation_tombstones()) {
        if (latest_ack_version >= version) continue;

        DeltaEntry entry;
        entry.operation = DeltaOperation::entity_add;
        entry.entity_id = static_cast<std::uint32_t>(id.value());
        delta_snapshot.entries.push_back(entry);
    }

    // New or modified components
    for (const auto &[identifier, version] : registry.get_component_metadata()) {
        if (latest_ack_version >= version) continue;

        ecs::Entity entity = identifier.first;
        std::type_index type_idx = identifier.second;

        // These tests should never fail, but just in case...
        if (std::find(current.entities.begin(), current.entities.end(), entity.value()) == current.entities.end()) {
            LOG_ERROR("Entity {} not found in current snapshot while computing delta", entity.value());
            continue;
        }
        if (std::find(current.entities[entity.value()].components.begin(),
                      current.entities[entity.value()].components.end(),
                      k_type_index_to_component_type_map.at(type_idx)) ==
            current.entities[entity.value()].components.end()) {
            LOG_ERROR("Component of type {} for entity {} not found in current snapshot while computing delta",
                static_cast<std::uint8_t>(k_type_index_to_component_type_map.at(type_idx)),
                entity.value());
            continue;
        }

        DeltaEntry entry;
        entry.operation = DeltaOperation::component_add_or_update;
        entry.entity_id = static_cast<std::uint32_t>(entity.value());
        entry.component = current.entities[entity.value()].components
            .at(k_type_index_to_component_type_map.at(type_idx));
        delta_snapshot.entries.push_back(entry);
    }

    // Deleted components
    for (const auto &[id, comp_and_version] : registry.get_component_destruction_tombstones()) {
        for (const auto &[component, version] : comp_and_version) {
            if (latest_ack_version >= version) continue;

            DeltaEntry entry;
            entry.operation = DeltaOperation::component_remove;
            entry.entity_id = static_cast<std::uint32_t>(id.value());
            entry.component.type = k_type_index_to_component_type_map.at(component);
            delta_snapshot.entries.push_back(entry);
        }
    }

    // Deleted entities
    for (const auto &[id, version] : registry.get_entity_destruction_tombstones()) {
        if (latest_ack_version >= version) continue;

        DeltaEntry entry;
        entry.operation = DeltaOperation::entity_remove;
        entry.entity_id = static_cast<std::uint32_t>(id.value());
        delta_snapshot.entries.push_back(entry);
    }

    return delta_snapshot;
}

void sys::send_snapshot_to_client(EngineContext& ctx,
    ecs::SparseArray<cpnt::Replicated> const& replicated_components)
{
    const auto &clients = ctx.get_clients();
    const auto &latest_snapshot = ctx.get_latest_snapshot();

    for (const auto &[id, endpoint] : clients) {
        const auto &ack_snapshot = ctx.get_latest_acknowledged_snapshot(id);

        auto delta_snapshot_opt = compute_delta(ack_snapshot.snapshot, latest_snapshot.last_update_tick, ctx.registry);
        if (!delta_snapshot_opt.has_value()) continue;

        WorldDelta world_delta = delta_snapshot_opt.value();
        std::unique_ptr<std::byte> data(world_delta.serialize());
        net::Packet packet;
        // TODO: create the packet and send it
    }
}
