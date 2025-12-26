#include "tag_registry.h"

#include <stdexcept>
#include <algorithm>

#include "utils/logger.h"

using namespace ecs;

TagRegistry::TagId TagRegistry::create_tag(const std::string &name) {
    if (m_name_to_id.find(name) != m_name_to_id.end()) {
        LOG_ERROR("Tag name {} already exists", name);
        return TagRegistry::k_invalid_tag_id;
    }

    TagId new_id = m_next_id++;
    m_name_to_id[name] = new_id;
    m_id_to_name[new_id] = name;
    return new_id;
}

void TagRegistry::bind_tag(TagId tag_id, Entity entity) {
    if (m_id_to_name.find(tag_id) == m_id_to_name.end()) {
        LOG_ERROR("Cannot bind TagId {} to the entity n°{}, TagId {} does not exists", tag_id, entity, tag_id);
        return;
    }

    m_tag_to_entity[tag_id] = entity;
}

TagRegistry::TagId TagRegistry::create_and_bind_tag(const std::string &name, Entity entity) {
    TagId tag_id = create_tag(name);

    if (tag_id == TagRegistry::k_invalid_tag_id)
        return TagRegistry::k_invalid_tag_id;
    bind_tag(tag_id, entity);
    return tag_id;
}

TagRegistry::TagId TagRegistry::get_tag_id(const std::string &name) const {
    auto it = m_name_to_id.find(name);
    if (it == m_name_to_id.end()) {
        LOG_ERROR("No TagId is linked to the name {}", name);
        return TagRegistry::k_invalid_tag_id;
    }
    return it->second;
}

TagRegistry::TagId TagRegistry::get_tag_id(Entity entity) const {
    auto it = std::find_if(m_tag_to_entity.begin(), m_tag_to_entity.end(),
        [&entity](const auto &p){ return p.second == entity; });

    if (it == m_tag_to_entity.end()) {
        LOG_ERROR("No TagId is linked to the entity n°{}", entity.value());
        return TagRegistry::k_invalid_tag_id;
    }
    return it->first;
}

std::string TagRegistry::get_tag_name(TagId tag_id) const {
    auto it = m_id_to_name.find(tag_id);
    if (it == m_id_to_name.end()) {
        LOG_ERROR("No name is linked to the TagId {}", tag_id);
        return "";
    }
    return it->second;
}

std::optional<Entity> TagRegistry::get_entity(TagId tag_id) const {
    auto it = m_tag_to_entity.find(tag_id);
    if (it == m_tag_to_entity.end()) {
        LOG_ERROR("No entity is bound to the tag_id {}", tag_id);
        return std::nullopt;
    }
    return it->second;
}

std::optional<Entity> TagRegistry::get_entity(const std::string &name) const {
    TagId tag_id = get_tag_id(name);

    if (tag_id == TagRegistry::k_invalid_tag_id) {
        LOG_ERROR("No entity is bound to the name {}", name);
        return std::nullopt;
    }
    return get_entity(tag_id);
}
