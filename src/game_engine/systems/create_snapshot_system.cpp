#include "systems/systems.h"

#include "ecs/zipper.h"
#include "engine.h"
#include "snapshots.h"

using namespace engn;

// Helper template to extract and cast a component to ISyncComponent
template<typename T>
std::shared_ptr<cpnt::ISyncComponent> extract_sync_component(const std::any& component_any) {
    // Try to cast as a raw component type
    if (auto comp_ptr = std::any_cast<T>(&component_any)) {
        // Check if it inherits from ISyncComponent using dynamic_cast
        if (auto* sync_ptr = dynamic_cast<const cpnt::ISyncComponent*>(comp_ptr)) {
            return std::make_shared<T>(*comp_ptr);
        }
    }
    return nullptr;
}

using Extractor = std::function<std::shared_ptr<cpnt::ISyncComponent>(const std::any&)>;

// Helper function to build the extractor map
static std::unordered_map<std::type_index, Extractor> build_sync_extractors() {
    return {
        {typeid(cpnt::Bullet), extract_sync_component<cpnt::Bullet>},
        {typeid(cpnt::Enemy), extract_sync_component<cpnt::Enemy>},
        {typeid(cpnt::Health), extract_sync_component<cpnt::Health>},
        {typeid(cpnt::Hitbox), extract_sync_component<cpnt::Hitbox>},
        {typeid(cpnt::MovementPattern), extract_sync_component<cpnt::MovementPattern>},
        {typeid(cpnt::Player), extract_sync_component<cpnt::Player>},
        {typeid(cpnt::Replicated), extract_sync_component<cpnt::Replicated>},
        {typeid(cpnt::Stats), extract_sync_component<cpnt::Stats>},
        {typeid(cpnt::Tag), extract_sync_component<cpnt::Tag>},
        {typeid(cpnt::Transform), extract_sync_component<cpnt::Transform>},
        {typeid(cpnt::Velocity), extract_sync_component<cpnt::Velocity>},
        {typeid(cpnt::Shooter), extract_sync_component<cpnt::Shooter>},
        {typeid(cpnt::BulletShooter), extract_sync_component<cpnt::BulletShooter>}
    };
}

static const std::unordered_map<std::type_index, Extractor> k_sync_extractors = build_sync_extractors();

void sys::create_snapshot_system(engn::EngineContext& engine_ctx,
    ecs::SparseArray<cpnt::Replicated> const& replicated_components) {
    auto& registry = engine_ctx.registry;
    WorldSnapshot snapshot;
    LOG_INFO("create_snapshot_system called");

    int repl_ent = 0;

    for (const auto &[idx, replicated] : ecs::indexed_zipper(replicated_components)) {
        EntitySnapshot entity_snapshot;
        entity_snapshot.entity_id = idx;

        const auto &components = registry.get_entity_components(registry.entity_from_index(idx));

        for (const auto &[type_idx, component_any] : components) {
            // Look up the extractor for this component type
            auto it = k_sync_extractors.find(type_idx);
            if (it != k_sync_extractors.end()) {
                auto sync_comp = it->second(component_any);
                if (sync_comp) {
                    SerializedComponent serialized_comp = sync_comp->serialize();
                    entity_snapshot.components.push_back(std::move(serialized_comp));
                }
            }
        }

        if (!entity_snapshot.components.empty())
            snapshot.entities.push_back(std::move(entity_snapshot));
        repl_ent++;
    }
    LOG_DEBUG("Found {} replicated entities", repl_ent);

    SnapshotRecord record;

    record.snapshot = std::move(snapshot);
    record.last_update_tick = static_cast<std::uint32_t>(engine_ctx.get_current_tick());
    record.acknowledged = false;
    engine_ctx.record_snapshot(record);
    LOG_INFO("New snapshot for tick {} & #{} entities", record.last_update_tick, record.snapshot.entities.size());
}
