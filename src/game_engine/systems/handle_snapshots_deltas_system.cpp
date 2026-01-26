#include "systems/systems.h"

#include "ecs/zipper.h"

#include "engine.h"

using namespace engn;

static void add_entity(ecs::Registry &registry, const DeltaEntry &entry);
static void remove_entity(EngineContext& ctx, ecs::Registry &registry, const DeltaEntry &entry);
static void add_component(ecs::Registry &registry, const DeltaEntry &entry); // Also manages modifications
static void remove_component(ecs::Registry &registry, const DeltaEntry &entry);
static void apply_player_sprite_variant(ecs::Registry &registry, ecs::Entity entity);
static void maybe_spawn_death_explosion(ecs::Registry &registry, ecs::Entity entity, int old_hp, int new_hp);

#pragma region Archetypes

constexpr float k_enemy_sprite_x = 5.0f;
constexpr float k_enemy_sprite_y = 6.0f;
constexpr float k_enemy_sprite_width = 21.0f;
constexpr float k_enemy_sprite_height = 23.0f;
constexpr float k_enemy_scale = 5.0f;

// Create player's entity (match solo)
constexpr float k_ship_sprite_x = 34.0f;
constexpr float k_ship_sprite_y = 0.0f;
constexpr float k_ship_width = 33.0f;
constexpr float k_ship_height = 18.0f;
constexpr float k_ship_scale = 3.0f;
constexpr float k_player_sprite_row_height = 17.0f;

// Bullet
constexpr float k_bullet_offset_x = 50.0f;
constexpr float k_bullet_offset_y = 30.0f;
constexpr float k_bullet_speed = 650.0f;
constexpr float k_bullet_width = 16.0f;
constexpr float k_bullet_height = 8.0f;
constexpr float k_bullet_sprite_x = 249.0f;
constexpr float k_bullet_sprite_y = 105.0f;
constexpr float k_bullet_scale = 2.0f;

// Shooter
constexpr float k_shooter_sprite_x = 87.0f;
constexpr float k_shooter_sprite_y = 67.0f;
constexpr float k_shooter_sprite_width = 22.0f;
constexpr float k_shooter_sprite_height = 18.0f;
constexpr float k_shooter_scale = 5.0f;

// Shooter bullets
constexpr float k_shooter_bullet_width = 16.0f;
constexpr float k_shooter_bullet_height = 8.0f;
constexpr float k_shooter_bullet_sprite_x = 249.0f;
constexpr float k_shooter_bullet_sprite_y = 105.0f;
constexpr float k_shooter_bullet_scale = 2.0f;

// Boss
constexpr float k_boss_sprite_x = 27.0f;
constexpr float k_boss_sprite_y = 861.0f;
constexpr float k_boss_sprite_width = 154.0f;
constexpr float k_boss_sprite_height = 203.0f;
constexpr float k_boss_scale = 5.0f;
constexpr float k_boss_wave_center_x = 1350.0f;
constexpr float k_boss_wave_center_y = 400.0f;
constexpr float k_boss_wave_speed = 2000.0f;

// Explosion (large)
constexpr float k_large_explosion_sprite_x = 0.0f;
constexpr float k_large_explosion_sprite_y = 99.0f;
constexpr float k_large_explosion_sprite_w = 65.0f;
constexpr float k_large_explosion_sprite_h = 64.0f;
constexpr float k_large_explosion_scale = 2.0f;
constexpr float k_large_explosion_frame_duration = 0.08f;
constexpr int k_large_explosion_frames = 5;

// Type-erased component removal function
using ComponentRemover = std::function<void(ecs::Registry&, ecs::Entity)>;

// Type-erased component adder function
using ComponentAdder = std::function<void(ecs::Registry&, ecs::Entity, const SerializedComponent&)>;


// Build a map from ComponentType to removal functions
static std::unordered_map<ComponentType, ComponentRemover> build_component_removers() {
    return {
        {ComponentType::bullet, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::Bullet>(e); }},
        {ComponentType::enemy, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::Enemy>(e); }},
        {ComponentType::entity_type, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::EntityType>(e); }},
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
        {ComponentType::bullet_shooter, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::BulletShooter>(e); }},
        {ComponentType::boss, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::Boss>(e); }},
        {ComponentType::boss_hitbox, [](ecs::Registry& reg, ecs::Entity e) { reg.remove_component<cpnt::BossHitbox>(e); }},
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
        {ComponentType::entity_type, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::EntityType component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
        {ComponentType::health, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            int old_hp = -1;
            const auto& healths = reg.get_components<cpnt::Health>();
            const auto k_index = static_cast<std::size_t>(static_cast<ecs::Entity::IdType>(e));
            if (k_index < healths.size() && healths[k_index].has_value()) {
                old_hp = healths[k_index]->hp;
            }
            cpnt::Health component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
            if (old_hp >= 0) {
                const auto& new_healths = reg.get_components<cpnt::Health>();
                if (k_index < new_healths.size() && new_healths[k_index].has_value()) {
                    maybe_spawn_death_explosion(reg, e, old_hp, new_healths[k_index]->hp);
                }
            }
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
        {ComponentType::bullet_shooter, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::BulletShooter component;
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
        {ComponentType::boss, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::Boss component{0.0f, 0.0f, 0.0f, false, false, {k_boss_wave_center_x, k_boss_wave_center_y}, 0.0f, k_boss_wave_speed};
            component.deserialize(sc.data);
            reg.add_component(e, std::move(component));
        }},
        {ComponentType::boss_hitbox, [](ecs::Registry& reg, ecs::Entity e, const SerializedComponent& sc) {
            cpnt::BossHitbox component{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
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
        ecs::Registry &registry = ctx.registry;

        LOG_DEBUG("[CLIENT] Applying snapshot delta with {} entries (base tick {})",
                  delta.entries.size(),
                  delta.base_snapshot_tick);
        for (const DeltaEntry &entry : delta.entries) {
            switch (entry.operation) {

                case (DeltaOperation::entity_add): add_entity(registry, entry); /*LOG_DEBUG("add_entity");*/ break;
                case (DeltaOperation::entity_remove): remove_entity(ctx, registry, entry); /*LOG_DEBUG("remove_entity");*/ break;
                case (DeltaOperation::component_add_or_update): add_component(registry, entry); /*LOG_DEBUG("add_component");*/ break;
                case (DeltaOperation::component_remove): remove_component(registry, entry); /*LOG_DEBUG("remove_component");*/ break;
            }
        }
    });
}

static void add_entity(ecs::Registry &registry, const DeltaEntry &entry)
{
    // Avoid duplicating entities if components arrived before the entity_add.
    for (const auto &[entity_id, replicated] : ecs::indexed_zipper(registry.get_components<cpnt::Replicated>())) {
        if (replicated != std::nullopt && replicated->tag == entry.entity_id) {
            LOG_DEBUG("[CLIENT] entity_add ignored for replicated ID {} (already exists as local entity {})",
                      entry.entity_id, static_cast<std::uint32_t>(registry.entity_from_index(entity_id)));
            return;
        }
    }

    auto id = registry.spawn_entity();

    // All entity created over the network will have the replicated tag
    // It helps make a relation between server entities ids & local ones
    registry.add_component(id, cpnt::Replicated{entry.entity_id});
    LOG_INFO("[CLIENT] Received entity_add for replicated ID {} -> local entity {}", entry.entity_id, static_cast<std::uint32_t>(id));
}

static void remove_entity(EngineContext& ctx, ecs::Registry &registry, const DeltaEntry &entry)
{
    auto replicated_id = entry.entity_id;

    for (const auto &[entity_id, replicated] : ecs::indexed_zipper(registry.get_components<cpnt::Replicated>())) {
        if (replicated != std::nullopt && replicated->tag == replicated_id) {
            auto entity = registry.entity_from_index(entity_id);

            if (registry.has_component<cpnt::Bullet>(entity) ||
                registry.has_component<cpnt::BulletShooter>(entity)) {
                auto& positions = registry.get_components<cpnt::Transform>();
                if (positions[entity_id].has_value()) {
                    auto& pos = positions[entity_id].value();
                    // NOLINTBEGIN(cppcoreguidelines-pro-type-union-access)
                    const bool k_on_screen = pos.x >= 0.0f && pos.x <= ctx.window_size.x &&
                                             pos.y >= 0.0f && pos.y <= ctx.window_size.y;
                    // NOLINTEND(cppcoreguidelines-pro-type-union-access)

                    if (k_on_screen) {
                        constexpr float k_explosion_sprite_x = 114.0f;
                        constexpr float k_explosion_sprite_y = 18.0f;
                        constexpr float k_explosion_sprite_w = 17.0f;
                        constexpr float k_explosion_sprite_h = 16.0f;
                        constexpr float k_explosion_scale = 3.0f;
                        constexpr float k_explosion_frame_duration = 0.08f;
                        constexpr int k_explosion_total_frames = 5;

                        auto explosion = registry.spawn_entity();
                        registry.add_component(explosion, cpnt::Transform{pos.x, pos.y, 0.0f, 0.0f,
                                                                     0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
                        registry.add_component(explosion, cpnt::Sprite{{k_explosion_sprite_x, k_explosion_sprite_y,
                                                                   k_explosion_sprite_w, k_explosion_sprite_h},
                                                                  k_explosion_scale,
                                                                  0,
                                                                  "bulletExplosion"});
                        registry.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
                        registry.add_component(explosion,
                                          cpnt::Explosion{cpnt::Explosion::ExplosionType::Small, 0.0f,
                                                          k_explosion_frame_duration, 0, k_explosion_total_frames});
                    }
                }
            }
            
            registry.kill_entity(entity);
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
    // LOG_WARNING("Could not find local entity with replicated id {} for component removal", replicated_id);
}

static void initialize_archetype(ecs::Registry &registry, ecs::Entity entity, const DeltaEntry& entry);
static void ensure_transform(ecs::Registry &registry, ecs::Entity entity, const char* archetype_name);
static void ensure_velocity(ecs::Registry &registry, ecs::Entity entity, const char* archetype_name);

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
    ecs::Entity local_entity{};
    bool found = false;
    for (const auto &[entity_id, replicated] : ecs::indexed_zipper(registry.get_components<cpnt::Replicated>())) {
        if (replicated != std::nullopt && replicated->tag == replicated_id) {
            local_entity = registry.entity_from_index(entity_id);
            found = true;
            break;
        }
    }

    if (!found) {
        // Create the entity if component updates arrived before entity_add.
        local_entity = registry.spawn_entity();
        registry.add_component(local_entity, cpnt::Replicated{replicated_id});
        LOG_DEBUG("[CLIENT] Created entity {} for replicated ID {} from component update",
                  static_cast<std::uint32_t>(local_entity), replicated_id);
    }

    auto it = k_component_adders.find(type);
    if (it != k_component_adders.end()) {
        it->second(registry, local_entity, serialized);

        // Log EntityType additions
        if (type == ComponentType::entity_type) {
            cpnt::EntityType temp_entity_type;
            temp_entity_type.deserialize(serialized.data);
            LOG_INFO("[CLIENT] Added EntityType '{}' to replicated ID {} (local entity {})",
                     temp_entity_type.type_name, replicated_id, static_cast<std::uint32_t>(local_entity));
            
            // Ensure Transform and Velocity exist BEFORE initializing archetype
            ensure_transform(registry, local_entity, "entity_creation");
            ensure_velocity(registry, local_entity, "entity_creation");
        }

        // Init graphics component if an EntityType was added
        if (type == ComponentType::entity_type) {
            initialize_archetype(registry, local_entity, entry);
        }
        if (type == ComponentType::player) {
            apply_player_sprite_variant(registry, local_entity);
        }
        if (type == ComponentType::shooter && !registry.has_component<cpnt::EntityType>(local_entity)) {
            ensure_transform(registry, local_entity, "shooter");
            ensure_velocity(registry, local_entity, "shooter");
            if (!registry.has_component<cpnt::Sprite>(local_entity)) {
                registry.add_component(
                    local_entity, cpnt::Sprite{{k_shooter_sprite_x, k_shooter_sprite_y,
                                                k_shooter_sprite_width, k_shooter_sprite_height},
                                               k_shooter_scale,
                                               0,
                                               "shooter_sprite"});
                LOG_INFO("[CLIENT] Shooter sprite added for entity {}", static_cast<std::uint32_t>(local_entity));
            }
        }
        if (type == ComponentType::bullet_shooter) {
            ensure_transform(registry, local_entity, "bullet_shooter");
            ensure_velocity(registry, local_entity, "bullet_shooter");
            if (!registry.has_component<cpnt::Sprite>(local_entity)) {
                registry.add_component(
                    local_entity, cpnt::Sprite{{k_shooter_bullet_sprite_x, k_shooter_bullet_sprite_y,
                                                k_shooter_bullet_width, k_shooter_bullet_height},
                                               k_shooter_bullet_scale,
                                               0,
                                               "shooter_bullet"});
                LOG_INFO("[CLIENT] Shooter bullet sprite added for entity {}", static_cast<std::uint32_t>(local_entity));
            }
        }
        if (type == ComponentType::boss) {
            ensure_transform(registry, local_entity, "boss");
            ensure_velocity(registry, local_entity, "boss");
            if (!registry.has_component<cpnt::Sprite>(local_entity)) {
                registry.add_component(
                    local_entity, cpnt::Sprite{{k_boss_sprite_x, k_boss_sprite_y, k_boss_sprite_width, k_boss_sprite_height},
                                               k_boss_scale,
                                               0,
                                               "boss"});
                LOG_INFO("[CLIENT] Boss sprite added for entity {}", static_cast<std::uint32_t>(local_entity));
            }
        }
    } else {
        LOG_WARNING("Unknown component type {} for addition",
            static_cast<std::uint8_t>(type));
    }
}

static void initialize_archetype(ecs::Registry &registry, ecs::Entity entity, const DeltaEntry& entry)
{
    // Check if entity has an EntityType component to determine its archetype
    if (!registry.has_component<cpnt::EntityType>(entity)) {
        return;
    }

    auto& components = registry.get_components<cpnt::EntityType>();
    auto entity_type_opt = components[entity];

    if (!entity_type_opt.has_value()) {
        return;
    }

    const auto& entity_type = entity_type_opt.value();

    // Initialize graphics components based on entity type
    if (entity_type.type_name == "player") {
        LOG_DEBUG("[CLIENT] Initializing player archetype for entity {}", static_cast<std::uint32_t>(entity));
        if (!registry.has_component<cpnt::Sprite>(entity)) {
            registry.add_component(
                entity, cpnt::Sprite{{k_ship_sprite_x, k_ship_sprite_y, k_ship_width, k_ship_height},
                                    k_ship_scale,
                                    0,
                                    "players"});
        }
        apply_player_sprite_variant(registry, entity);
    } else if (entity_type.type_name == "charger") {
        LOG_DEBUG("[CLIENT] Initializing charger archetype for entity {}", static_cast<std::uint32_t>(entity));
        if (!registry.has_component<cpnt::Sprite>(entity)) {
            registry.add_component(
                entity, cpnt::Sprite{{k_enemy_sprite_x, k_enemy_sprite_y, k_enemy_sprite_width, k_enemy_sprite_height},
                                    k_enemy_scale,
                                    0,
                                    "enemy_ship"});
        }
    } else if (entity_type.type_name == "shooter") {
        LOG_DEBUG("[CLIENT] Initializing shooter archetype for entity {}", static_cast<std::uint32_t>(entity));
        if (!registry.has_component<cpnt::Sprite>(entity)) {
            registry.add_component(
                entity, cpnt::Sprite{{k_shooter_sprite_x, k_shooter_sprite_y,
                                      k_shooter_sprite_width, k_shooter_sprite_height},
                                     k_shooter_scale,
                                     0,
                                     "shooter_sprite"});
        }
    } else if (entity_type.type_name == "bullet") {
        LOG_DEBUG("[CLIENT] Initializing bullet archetype for entity {}", static_cast<std::uint32_t>(entity));
        if (!registry.has_component<cpnt::Sprite>(entity)) {
            registry.add_component(
                entity, cpnt::Sprite{{k_bullet_sprite_x, k_bullet_sprite_y, k_bullet_width, k_bullet_height},
                                     k_bullet_scale,
                                     0,
                                     "player_ship"});
        }
    } // etc
}

#pragma endregion Archetypes

static void ensure_transform(ecs::Registry &registry, ecs::Entity entity, const char* archetype_name)
{
    if (!registry.has_component<cpnt::Transform>(entity)) {
        registry.add_component(entity, cpnt::Transform{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
        LOG_WARNING("[CLIENT] Missing Transform for {} entity {}, using default transform",
                    archetype_name,
                    static_cast<std::uint32_t>(entity));
    }
}

static void ensure_velocity(ecs::Registry &registry, ecs::Entity entity, const char* archetype_name)
{
    if (!registry.has_component<cpnt::Velocity>(entity)) {
        registry.add_component(entity, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
        LOG_WARNING("[CLIENT] Missing Velocity for {} entity {}, using default velocity",
                    archetype_name,
                    static_cast<std::uint32_t>(entity));
    }
}

static void apply_player_sprite_variant(ecs::Registry &registry, ecs::Entity entity)
{
    if (!registry.has_component<cpnt::Player>(entity) || !registry.has_component<cpnt::Sprite>(entity)) {
        return;
    }

    const auto& players = registry.get_components<cpnt::Player>();
    const auto& sprites = registry.get_components<cpnt::Sprite>();
    if (!players[entity].has_value() || !sprites[entity].has_value()) {
        return;
    }

    auto& sprite = registry.get_components<cpnt::Sprite>()[entity];
    sprite->source_rect.y = k_player_sprite_row_height * static_cast<float>(players[entity]->id);
}

static void maybe_spawn_death_explosion(ecs::Registry &registry, ecs::Entity entity, int old_hp, int new_hp)
{
    if ((old_hp >= 0 && old_hp <= 0) || new_hp > 0) {
        return;
    }
    if (!registry.has_component<cpnt::Transform>(entity)) {
        return;
    }

    const auto k_index = static_cast<std::size_t>(static_cast<ecs::Entity::IdType>(entity));
    const auto& positions = registry.get_components<cpnt::Transform>();
    if (k_index >= positions.size() || !positions[k_index].has_value()) {
        return;
    }

    bool is_large = registry.has_component<cpnt::Enemy>(entity) ||
                    registry.has_component<cpnt::Shooter>(entity) ||
                    registry.has_component<cpnt::Boss>(entity) ||
                    registry.has_component<cpnt::Player>(entity);
    if (!is_large) {
        return;
    }

    const auto& pos = positions[k_index].value();
    auto explosion = registry.spawn_entity();
    registry.add_component(explosion, cpnt::Transform{pos.x, pos.y, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f});
    registry.add_component(explosion, cpnt::Sprite{{k_large_explosion_sprite_x, k_large_explosion_sprite_y,
                                                    k_large_explosion_sprite_w, k_large_explosion_sprite_h},
                                                   k_large_explosion_scale,
                                                   0,
                                                   "explosion"});
    registry.add_component(explosion, cpnt::Velocity{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f});
    registry.add_component(explosion,
                           cpnt::Explosion{cpnt::Explosion::ExplosionType::Large, 0.0f,
                                           k_large_explosion_frame_duration, 0, k_large_explosion_frames});

    if (registry.has_component<cpnt::Player>(entity)) {
        registry.remove_component<cpnt::Sprite>(entity);
    }
}
