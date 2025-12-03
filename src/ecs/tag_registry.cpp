#include "tag_registry.h"

using namespace ecs;

#include <stdexcept>

TagRegistry::TagId TagRegistry::create_tag(const std::string &name) {
    if (m_name_to_id.find(name) != m_name_to_id.end())
        throw std::runtime_error("Tag name already exists: " + name);

    TagId new_id = m_next_id++;
    m_name_to_id[name] = new_id;
    m_id_to_name[new_id] = name;
    return new_id;
}

void TagRegistry::bind_tag(TagId tag_id, Entity entity) {
    if (m_id_to_name.find(tag_id) == m_id_to_name.end())
        throw std::runtime_error("Tag ID does not exist: " + std::to_string(tag_id));

    m_tag_to_entity[tag_id] = entity;
}

TagRegistry::TagId TagRegistry::create_and_bind_tag(const std::string &name, Entity entity) {
    TagId tag_id = create_tag(name);
    bind_tag(tag_id, entity);
    return tag_id;
}

TagRegistry::TagId TagRegistry::get_tag_id(const std::string &name) const {
    auto it = m_name_to_id.find(name);
    if (it == m_name_to_id.end())
        throw std::runtime_error("Tag name does not exist: " + name);
    return it->second;
}

std::string TagRegistry::get_tag_name(TagId tag_id) const {
    auto it = m_id_to_name.find(tag_id);
    if (it == m_id_to_name.end())
        throw std::runtime_error("Tag ID does not exist: " + std::to_string(tag_id));
    return it->second;
}

Entity TagRegistry::get_entity(TagId tag_id) const {
    auto it = m_tag_to_entity.find(tag_id);
    if (it == m_tag_to_entity.end())
        throw std::runtime_error("Tag ID not bound to any entity: " + std::to_string(tag_id));
    return it->second;
}

Entity TagRegistry::get_entity(const std::string &name) const {
    TagId tag_id = get_tag_id(name);
    return get_entity(tag_id);
}
