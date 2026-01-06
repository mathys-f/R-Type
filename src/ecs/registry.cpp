#include "registry.h"

#include "utils/logger.h"

using namespace ecs;

Registry::EntityType Registry::spawn_entity() {
    if (!m_free_entities.empty()) {
        EntityType e = m_free_entities.back();
        m_free_entities.pop_back();
        return e;
    }
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
    m_free_entities.push_back(e);
}

const std::unordered_map<std::type_index, std::any>&
ecs::Registry::get_entity_components(Entity entity) const noexcept {
    // Use a thread-local static map to store the result and return by reference
    thread_local std::unordered_map<std::type_index, std::any> entity_components;
    entity_components.clear();

    // Iterate through all registered component types and extract components for this entity
    auto type_it = m_components_arrays.begin();
    for (size_t i = 0; i < m_extract_functions.size() && type_it != m_components_arrays.end(); ++i, ++type_it) {
        if (m_extract_functions[i]) {
            auto component = m_extract_functions[i](*this, entity);
            if (component.has_value()) {
                entity_components[type_it->first] = std::move(component.value());
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
