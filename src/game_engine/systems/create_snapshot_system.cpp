#include "systems/systems.h"

#include "ecs/zipper.h"
#include "engine.h"
#include "snapshots.h"

using namespace engn;

void sys::create_snapshot_system(engn::EngineContext& engine_ctx,
    ecs::SparseArray<cpnt::Replicated> const& replicated_components) {
    auto& registry = engine_ctx.registry;
    WorldSnapshot snapshot;

    for (const auto &[idx, replicated] : ecs::indexed_zipper(replicated_components)) {
        EntitySnapshot entity_snapshot;
        entity_snapshot.entity_id = replicated->tag;

        const auto &components = registry.get_entity_components(registry.entity_from_index(idx));

        for (const auto &[type_idx, component_any] : components) {
            // Check if component_any is derived from ISyncComponent
            if (auto sync_comp = std::any_cast<std::shared_ptr<cpnt::ISyncComponent>>(&component_any)) {
                SerializedComponent serialized_comp;
                serialized_comp = (*sync_comp)->serialize();
                entity_snapshot.components.push_back(std::move(serialized_comp));
            }
        }

        snapshot.entities.push_back(std::move(entity_snapshot));
    }
    engine_ctx.record_snapshot(std::move(snapshot));
}
