#pragma once

#include <string>
#include <unordered_map>

#include "entity.h"

namespace ecs {

class TagRegistry {
 public:
    using TagId = std::size_t;

    TagRegistry() = default;
    ~TagRegistry() = default;

    TagRegistry(TagRegistry const&) = delete; // Disable copy constructor
    TagRegistry const& operator=(TagRegistry const&) = delete; // Disable copy assignment

    TagRegistry(TagRegistry&&) = delete; // Disable move constructor
    TagRegistry const& operator=(TagRegistry&&) = delete; // Disable move assignment


    TagId create_tag(const std::string &name);
    void bind_tag(TagId tag_id, Entity entity);

    TagId create_and_bind_tag(const std::string &name, Entity entity);

    TagId get_tag_id(const std::string &name) const;
    std::string get_tag_name(TagId tag_id) const;

    Entity get_entity(const std::string &name) const;
    Entity get_entity(TagId tag_id) const;

 private:
    std::unordered_map<std::string, TagId> m_name_to_id;
    std::unordered_map<TagId, std::string> m_id_to_name;
    std::unordered_map<TagId, Entity> m_tag_to_entity;

    TagId m_next_id = 0;
};

} // namespace ecs
