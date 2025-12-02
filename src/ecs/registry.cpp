#include "registry.h"

using namespace ecs;

Registry::EntityType Registry::spawn_entity()
{
    if (!m_free_entities.empty())
    {
        EntityType e = m_free_entities.back();
        m_free_entities.pop_back();
        return e;
    }
    return EntityType{m_next_entity++};
}

Registry::EntityType Registry::entity_from_index(std::size_t idx) const noexcept
{
    return EntityType{static_cast<Entity::IdType>(idx)};
}

void Registry::kill_entity(EntityType const& e)
{
    for (auto& fn : m_erase_functions)
    {
        if (fn)
            fn(*this, e);
    }
    m_free_entities.push_back(e);
}

void Registry::run_systems()
{
    for (auto& sys : m_systems)
    {
        if (sys)
            sys(*this);
    }
}
