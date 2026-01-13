#include "registry.h"

#include "utils/logger.h"

using namespace ecs;

void ecs::Registry::set_current_version(Version v) noexcept {
    m_current_version = v;
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
    for (auto it = m_component_metadata.begin(); it != m_component_metadata.end(); ) {
        if (it->first.first == e) {
            it = m_component_metadata.erase(it);
        } else {
            ++it;
        }
    }
}

Registry::EntityType Registry::spawn_entity() {
    if (!m_free_entities.empty()) {
        EntityType e = m_free_entities.back();
        m_free_entities.pop_back();
        return e;
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

const std::unordered_map<std::type_index, std::any>&
ecs::Registry::get_entity_components(Entity entity) const noexcept {
    // Use a thread-local static map to store the result and return by reference
    thread_local std::unordered_map<std::type_index, std::any> s_entity_components;
    s_entity_components.clear();

    // Iterate through all registered component types and extract components for this entity
    auto type_it = m_components_arrays.begin();
    for (size_t i = 0; i < m_extract_functions.size() && type_it != m_components_arrays.end(); ++i, ++type_it) {
        if (m_extract_functions[i]) {
            auto component = m_extract_functions[i](*this, entity);
            if (component.has_value()) {
                s_entity_components[type_it->first] = std::move(component.value());
            }
        }
    }

    return s_entity_components;
}

TagRegistry& Registry::get_tag_registry() noexcept {
    return tag_registry;
}

const TagRegistry& Registry::get_tag_registry() const noexcept {
    return tag_registry;
}

