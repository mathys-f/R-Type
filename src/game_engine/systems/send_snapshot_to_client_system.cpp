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

        // Find the entity in the snapshot
        auto entity_it = std::find_if(current.entities.begin(), current.entities.end(),
            [entity](const EntitySnapshot& snapshot) {
                return snapshot.entity_id == entity.value();
            }
        );

        // Ensure the entity exists in the snapshot
        if ( entity_it == current.entities.end()) {
            LOG_ERROR("Entity {} not found in current snapshot while computing delta", entity.value());
            continue;
        }

        // Find the component in the entity
        auto comp_it = std::find_if(entity_it->components.begin(), entity_it->components.end(),
            [type_idx](const SerializedComponent& comp) {
                return comp.type == k_type_index_to_component_type_map.at(type_idx);
            }
        );

        // Ensure the component exists in the entity
        if (comp_it == entity_it->components.end()) {
            LOG_ERROR("Component of type {} for entity {} not found in current snapshot while computing delta",
                static_cast<std::uint8_t>(k_type_index_to_component_type_map.at(type_idx)),
                entity.value());
            continue;
        }

        DeltaEntry entry;
        entry.operation = DeltaOperation::component_add_or_update;
        entry.entity_id = static_cast<std::uint32_t>(entity.value());
        entry.component = *comp_it;
        delta_snapshot.entries.push_back(entry);
    }

    // Deleted components
    for (const auto &[id, comp_and_version] : registry.get_component_destruction_tombstones()) {
        for (const auto &[component, version] : comp_and_version) {
            if (latest_ack_version >= version) continue;

            DeltaEntry entry;
            entry.operation = DeltaOperation::component_remove;
            entry.entity_id = static_cast<std::uint32_t>(id.value());
            entry.component_type = k_type_index_to_component_type_map.at(component);
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

    for (const auto &endpoint : clients) {
        const auto &ack_snapshot = ctx.get_latest_acknowledged_snapshot(endpoint);
        auto &latest_snapshot = ctx.get_latest_snapshot(endpoint);

        auto delta_snapshot_opt = compute_delta(ack_snapshot.snapshot, latest_snapshot.last_update_tick, ctx.registry);
        if (!delta_snapshot_opt.has_value()) continue;

        WorldDelta world_delta = delta_snapshot_opt.value();
        std::unique_ptr<std::byte> data(world_delta.serialize());
        net::Packet packet;
        packet.header.m_command = static_cast<std::uint8_t>(net::CommandId::KServerEntityState);
        packet.header.m_payload_size = static_cast<std::uint16_t>(world_delta.get_serialized_size());
        packet.payload = std::vector<std::byte>(data.get(), data.get() + world_delta.get_serialized_size()); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        data.release(); // Prevent freing the data since it's now owned by the packet payload
        std::uint32_t packet_id = ctx.network_session->send(packet, endpoint, true);
        latest_snapshot.msg_id = packet_id;
        LOG_DEBUG("Sent snapshot delta to client {}:{}, base_snapshot_tick {}",
            endpoint.address().to_string(), endpoint.port(), latest_snapshot.last_update_tick);
    }
}
