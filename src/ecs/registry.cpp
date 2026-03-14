#include "registry.h"

#include "utils/logger.h"

using namespace ecs;

void ecs::Registry::set_current_version(Version v) noexcept {
    m_current_version = v;
}

ecs::Registry::Version ecs::Registry::get_current_version() const noexcept {
    return m_current_version;
}

const std::unordered_map<Registry::EntityType, Registry::Version>& ecs::Registry::get_entity_creation_tombstones() const noexcept {
    return m_entity_creation_tumbstones;
}

const std::unordered_map<Registry::EntityType, Registry::Version>& ecs::Registry::get_entity_destruction_tombstones() const noexcept {
    return m_entity_destruction_tumbstones;
}

const std::unordered_map<Registry::EntityType, std::unordered_map<std::type_index, Registry::Version>>&
ecs::Registry::get_component_destruction_tombstones() const noexcept {
    return m_component_destruction_tombstones;
}

const std::unordered_map<std::pair<Registry::EntityType, std::type_index>, Registry::Version>&
ecs::Registry::get_component_metadata() const noexcept {
    return m_component_metadata;
}

void ecs::Registry::remove_entity_creation_tombstone(EntityType const& e) {
    if (m_entity_creation_tumbstones.find(e) != m_entity_creation_tumbstones.end())
        m_entity_creation_tumbstones.erase(e);
}

void ecs::Registry::remove_entity_destruction_tombstone(EntityType const& e) {
    if (m_entity_destruction_tumbstones.find(e) != m_entity_destruction_tumbstones.end()) {
        m_entity_destruction_tumbstones.erase(e);
        remove_entity_components_metadata(e);
    }
}

void ecs::Registry::remove_component_destruction_tombstone(EntityType const& e, std::type_index const& ti) {
    if (m_component_destruction_tombstones.find(e) != m_component_destruction_tombstones.end()
        && m_component_destruction_tombstones.find(e)->second.find(ti) != m_component_destruction_tombstones.find(e)->second.end()) {
        m_component_destruction_tombstones.find(e)->second.erase(ti);
        remove_component_metadata(e, ti);
    }
}

void ecs::Registry::remove_component_metadata(EntityType const& e, std::type_index const& ti) {
    if (m_component_metadata.find({e, ti}) != m_component_metadata.end())
        m_component_metadata.erase({e, ti});
}

void ecs::Registry::remove_entity_components_metadata(EntityType const& e) {
    auto it = m_entity_to_components.find(e);
    if (it != m_entity_to_components.end()) {
        for (const auto& type_idx : it->second) {
            m_component_metadata.erase({e, type_idx});
        }
        m_entity_to_components.erase(it);
    }
}

Registry::EntityType Registry::spawn_entity() {
    if (!m_free_entities.empty()) {
        std::vector<EntityType> deferred;
        while (!m_free_entities.empty()) {
            EntityType e = m_free_entities.back();
            m_free_entities.pop_back();

            const bool k_has_destroy_tombstone = m_entity_destruction_tumbstones.find(e) != m_entity_destruction_tumbstones.end();
            const bool k_has_component_tombstones =
                m_component_destruction_tombstones.find(e) != m_component_destruction_tombstones.end();
            const bool k_has_metadata = m_entity_to_components.find(e) != m_entity_to_components.end();

            if (k_has_destroy_tombstone || k_has_component_tombstones || k_has_metadata) {
                // Defer reuse until tombstones/metadata are cleared to keep snapshots consistent.
                deferred.push_back(e);
                continue;
            }

            for (const auto& deferred_entity : deferred) {
                m_free_entities.push_back(deferred_entity);
            }
            return e;
        }

        for (const auto& deferred_entity : deferred) {
            m_free_entities.push_back(deferred_entity);
        }
    }
    m_entity_creation_tumbstones[EntityType{m_next_entity}] = m_current_version;
    return EntityType{m_next_entity++};
}

Registry::EntityType Registry::entity_from_index(std::size_t idx) const noexcept {
    return EntityType{static_cast<Entity::IdType>(idx)};
}

void Registry::kill_entity(EntityType const& e) {
    for (auto& fn : m_erase_functions) {
        if (fn)
            fn(*this, e);
    }
    m_entity_destruction_tumbstones[e] = m_current_version;
    m_free_entities.push_back(e);
}

void Registry::kill_entity_deferred(EntityType const& e) {
    m_deferred_kills.push_back(e);
}

void Registry::process_deferred_kills() {
    for (auto const& e : m_deferred_kills) {
        kill_entity(e);
    }
    m_deferred_kills.clear();
}

std::unordered_map<std::type_index, std::any>
ecs::Registry::get_entity_components(Entity entity) const noexcept {
    std::unordered_map<std::type_index, std::any> entity_components;

    // Iterate through all registered component types and extract components for this entity
    for (const auto& [type_idx, component_array] : m_components_arrays) {
        // Look up the extractor
        auto extract_it = m_extract_functions.find(type_idx);

        if (extract_it != m_extract_functions.end() && extract_it->second) {
            auto component = extract_it->second(*this, entity);
            if (component.has_value()) {
                entity_components[type_idx] = std::move(component.value());
            }
        }
    }

    return entity_components;
}

TagRegistry& Registry::get_tag_registry() noexcept {
    return tag_registry;
}

const TagRegistry& Registry::get_tag_registry() const noexcept {
    return tag_registry;
}

void Registry::clear() {
    m_components_arrays.clear();
    m_erase_functions.clear();
    m_extract_functions.clear();
    m_systems.clear();
    m_next_entity = 0;
    m_free_entities.clear();
    m_deferred_kills.clear();
    m_current_version = 1;
    m_entity_creation_tumbstones.clear();
    m_entity_destruction_tumbstones.clear();
    m_component_destruction_tombstones.clear();
    m_component_metadata.clear();
    m_entity_to_components.clear();
    tag_registry.clear();
}
