#include "systems/systems.h"

#include "ecs/zipper.h"

#include "engine.h"

using namespace engn;

static void add_entity(ecs::Registry &registry, const DeltaEntry &entry);
static void remove_entity(ecs::Registry &registry, const DeltaEntry &entry);
static void add_component(ecs::Registry &registry, const DeltaEntry &entry);
static void remove_component(ecs::Registry &registry, const DeltaEntry &entry);

void sys::handle_snapshots_deltas_system(EngineContext& ctx)
{
    ctx.for_each_snapshot_delta([](EngineContext &ctx, const WorldDelta& delta) {
        LOG_DEBUG("Processing snapshot delta based on tick {}", delta.base_snapshot_tick);
        LOG_DEBUG("Entries count: {}", delta.entries.size());
        ecs::Registry &registry = ctx.registry;

        for (const DeltaEntry &entry : delta.entries) {
            switch (entry.operation) {

                case (DeltaOperation::entity_add): add_entity(registry, entry); break;
                case (DeltaOperation::entity_remove): remove_entity(registry, entry); break;
                case (DeltaOperation::component_add_or_update): add_component(registry, entry); break;
                case (DeltaOperation::component_remove): remove_component(registry, entry); break;
            }
        }
    });
}

static void add_entity(ecs::Registry &registry, const DeltaEntry &entry)
{
    auto id = registry.spawn_entity();

    // All entity created over the network will have the replicated tag
    // It helps make a relation between server entities ids & local ones
    registry.add_component(id, cpnt::Replicated{entry.entity_id});
    LOG_DEBUG("Add entity id:{}  repl_id:{}",id, entry.entity_id);
}

static void remove_entity(ecs::Registry &registry, const DeltaEntry &entry)
{
    auto replicated_id = entry.entity_id;

    for (const auto &[entity_id, replicated] : ecs::indexed_zipper(registry.get_components<cpnt::Replicated>())) {
        if (replicated != std::nullopt && replicated->tag == replicated_id) {
            registry.kill_entity(registry.entity_from_index(entity_id));
            LOG_DEBUG("Kill entity id:{}  repl_id:{}", entity_id, replicated_id);
            return;
        }
    }
}

static void add_component(ecs::Registry &registry, const DeltaEntry &entry)
{
    
}

static void remove_component(ecs::Registry &registry, const DeltaEntry &entry)
{
}

