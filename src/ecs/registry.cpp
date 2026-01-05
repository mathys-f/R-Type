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

const std::unordered_map<std::type_index, std::any>& Registry::dump_components() const noexcept {
    return m_components_arrays;
}

TagRegistry& Registry::get_tag_registry() noexcept {
    return tag_registry;
}

const TagRegistry& Registry::get_tag_registry() const noexcept {
    return tag_registry;
}
