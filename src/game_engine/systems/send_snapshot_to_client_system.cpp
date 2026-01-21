#include "systems/systems.h"

#include <memory>

#include "networking/rtp/networking.h"

#include "engine.h"
#include "snapshots.h"
#include "utils/logger.h"

using namespace engn;

static std::optional<WorldDelta> compute_delta(WorldSnapshot const& snapshot,
    ecs::Registry::Version latest_ack_version,
    const ecs::Registry &registry)
{
    WorldDelta delta;

    if (latest_ack_version == 0) {
        for (const auto& entity_snapshot : snapshot.entities) {
            DeltaEntry entity_entry;
            entity_entry.operation = DeltaOperation::entity_add;
            entity_entry.entity_id = entity_snapshot.entity_id;
            delta.entries.push_back(entity_entry);

            for (const auto& component : entity_snapshot.components) {
                DeltaEntry comp_entry;
                comp_entry.operation = DeltaOperation::component_add_or_update;
                comp_entry.entity_id = entity_snapshot.entity_id;
                comp_entry.component = component;
                delta.entries.push_back(comp_entry);
            }
        }
        
        if (delta.entries.size() == 0) return std::nullopt;
        return delta;
    }

    // New entities
    for (const auto &[id, version] : registry.get_entity_creation_tombstones()) {
        if (latest_ack_version >= version) continue;

        DeltaEntry entry;
        entry.operation = DeltaOperation::entity_add;
        entry.entity_id = static_cast<std::uint32_t>(id.value());
        delta.entries.push_back(entry);
    }

    // New or modified components
    for (const auto &[identifier, version] : registry.get_component_metadata()) {
        if (latest_ack_version >= version) continue;

        ecs::Entity entity = identifier.first;
        std::type_index type_idx = identifier.second;

        // Find the entity in the snapshot
        auto entity_it = std::find_if(snapshot.entities.begin(), snapshot.entities.end(),
            [entity](const EntitySnapshot& snapshot) {
                return snapshot.entity_id == entity.value();
            }
        );

        // Ensure the entity exists in the snapshot
        if (entity_it == snapshot.entities.end()) {
            // This can happen for entities created before version tracking started
            // Skip this component update - the entity will be sent in the next full sync or when properly created
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
                type_idx.name(), entity.value());
            continue;
        }

        DeltaEntry entry;
        entry.operation = DeltaOperation::component_add_or_update;
        entry.entity_id = static_cast<std::uint32_t>(entity.value());
        entry.component = *comp_it;
        delta.entries.push_back(entry);
    }

    // Deleted components
    for (const auto &[id, comp_and_version] : registry.get_component_destruction_tombstones()) {
        for (const auto &[component, version] : comp_and_version) {
            if (latest_ack_version >= version) continue;

            DeltaEntry entry;
            entry.operation = DeltaOperation::component_remove;
            entry.entity_id = static_cast<std::uint32_t>(id.value());
            entry.component_type = k_type_index_to_component_type_map.at(component);
            delta.entries.push_back(entry);
        }
    }

    // Deleted entities
    for (const auto &[id, version] : registry.get_entity_destruction_tombstones()) {
        if (latest_ack_version >= version) continue;

        DeltaEntry entry;
        entry.operation = DeltaOperation::entity_remove;
        entry.entity_id = static_cast<std::uint32_t>(id.value());
        delta.entries.push_back(entry);
    }

    if (delta.entries.size() == 0) return std::nullopt;

    return delta;
}

void sys::send_snapshot_to_client_system(EngineContext& ctx,
    ecs::SparseArray<cpnt::Replicated> const& replicated_components)
{
    // LOG_DEBUG("Running send_snapshot_to_client_system");
    const auto k_clients = ctx.get_clients();

    for (const auto &endpoint : k_clients) {
        const auto &ack_snapshot = ctx.get_latest_acknowledged_snapshot(endpoint);
        auto &latest_snapshot = ctx.get_latest_snapshot(endpoint);

        auto delta_snapshot_opt = compute_delta(latest_snapshot.snapshot, ack_snapshot.last_update_tick, ctx.registry);
        if (!delta_snapshot_opt.has_value()) continue;

        WorldDelta world_delta = delta_snapshot_opt.value();
        world_delta.base_snapshot_tick = ctx.get_current_tick();
        std::unique_ptr<std::byte> data(world_delta.serialize());
        net::Packet packet;
        packet.header.m_command = static_cast<std::uint8_t>(net::CommandId::KServerEntityState);
        packet.header.m_payload_size = static_cast<std::uint16_t>(world_delta.get_serialized_size());
        packet.payload = std::vector<std::byte>(data.get(), data.get() + world_delta.get_serialized_size()); // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        data.release(); // Prevent freing the data since it's now owned by the packet payload
        std::uint32_t packet_id = ctx.network_session->send(packet, endpoint, true);
        latest_snapshot.msg_id = packet_id;
    }
}
