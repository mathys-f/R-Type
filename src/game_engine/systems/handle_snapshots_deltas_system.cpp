#include "systems/systems.h"

#include "ecs/zipper.h"

#include "engine.h"

using namespace engn;

static void add_entity(ecs::Registry &registry, const DeltaEntry &entry);
static void remove_entity(ecs::Registry &registry, const DeltaEntry &entry);
static void add_component(ecs::Registry &registry, const DeltaEntry &entry); // Also manages modifications
static void remove_component(ecs::Registry &registry, const DeltaEntry &entry);

// Type-erased component removal function
using ComponentRemover = std::function<void(ecs::Registry&, ecs::Entity)>;

// Type-erased component adder function
using ComponentAdder = std::function<void(ecs::Registry&, ecs::Entity, const SerializedComponent&)>;


// Build a map from ComponentType to removal functions
static std::unordered_map<ComponentType, ComponentRemover> build_component_removers() {
    return {
        {ComponentType::bullet, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::Bullet>(e); }},
        {ComponentType::enemy, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::Enemy>(e); }},
        {ComponentType::health, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::Health>(e); }},
        {ComponentType::hitbox, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::Hitbox>(e); }},
        {ComponentType::movement_pattern, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::MovementPattern>(e); }},
        {ComponentType::player, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::Player>(e); }},
        {ComponentType::replicated, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::Replicated>(e); }},
        {ComponentType::stats, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::Stats>(e); }},
        {ComponentType::tag, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::Tag>(e); }},
        {ComponentType::transform, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::Transform>(e); }},
        {ComponentType::velocity, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::Velocity>(e); }},
        {ComponentType::shooter, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::Shooter>(e); }},
        {ComponentType::BulletShooter, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::BulletShooter>(e); }},
    };
}

// Build a map from ComponentType to adder functions
static std::unordered_map<ComponentType, ComponentAdder> build_component_adders() {
    return {
        {ComponentType::bullet, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::Bullet component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
        {ComponentType::enemy, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::Enemy component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
        {ComponentType::health, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::Health component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
        {ComponentType::hitbox, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::Hitbox component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
        {ComponentType::movement_pattern, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::MovementPattern component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
        {ComponentType::player, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::Player component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
        {ComponentType::replicated, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::Replicated component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
        {ComponentType::stats, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::Stats component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
        {ComponentType::tag, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::Tag component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
        {ComponentType::transform, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::Transform component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
        {ComponentType::velocity, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::Velocity component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
        {ComponentType::shooter, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::Shooter component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
        {ComponentType::BulletShooter, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::BulletShooter component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
    };
}

static const std::unordered_map<ComponentType, ComponentRemover> k_component_removers = build_component_removers();
static const std::unordered_map<ComponentType, ComponentAdder> k_component_adders = build_component_adders();

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

static void remove_component(ecs::Registry &registry, const DeltaEntry &entry)
{
    ComponentType type = entry.component_type;
    auto replicated_id = entry.entity_id;

    // Find the local entity that has this replicated id
    for (const auto &[entity_id, replicated] : ecs::indexed_zipper(registry.get_components<cpnt::Replicated>())) {
        if (replicated != std::nullopt && replicated->tag == replicated_id) {
            auto local_entity = registry.entity_from_index(entity_id);

            auto it = k_component_removers.find(type);
            if (it != k_component_removers.end()) {
                it->second(registry, local_entity);
            } else {
                LOG_WARNING("Unknown component type {} for removal",
                    static_cast<std::uint8_t>(type));
            }
            return;
        }
    }
    LOG_WARNING("Could not find local entity with replicated id {} for component removal", replicated_id);
}

static void add_component(ecs::Registry &registry, const DeltaEntry &entry)
{
    if (!entry.component.has_value()) {
        LOG_WARNING("Received an add_component with a null component");
        return;
    }

    const SerializedComponent& serialized = entry.component.value();
    ComponentType type = serialized.type;
    auto replicated_id = entry.entity_id;

    // Find the local entity that has this replicated id
    for (const auto &[entity_id, replicated] : ecs::indexed_zipper(registry.get_components<cpnt::Replicated>())) {
        if (replicated != std::nullopt && replicated->tag == replicated_id) {
            auto local_entity = registry.entity_from_index(entity_id);

            auto it = k_component_adders.find(type);
            if (it != k_component_adders.end()) {
                it->second(registry, local_entity, serialized);
            } else {
                LOG_WARNING("Unknown component type {} for addition",
                    static_cast<std::uint8_t>(type));
            }
            return;
        }
    }
    LOG_WARNING("Could not find local entity with replicated id {} for component addition", replicated_id);
}
