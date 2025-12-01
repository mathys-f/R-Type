#include "registry.h"

Registry::entity_t Registry::spawn_entity()
{
    if (!_free_entities.empty())
    {
        entity_t e = _free_entities.back();
        _free_entities.pop_back();
        return e;
    }
    return entity_t{_next_entity++};
}

Registry::entity_t Registry::entity_from_index(std::size_t idx) const noexcept
{
    return entity_t{static_cast<Entity::IdType>(idx)};
}

void Registry::kill_entity(entity_t const &e)
{
    for (auto &fn : _erase_functions)
    {
        if (fn)
            fn(*this, e);
    }
    _free_entities.push_back(e);
}

void Registry::run_systems()
{
    for (auto &sys : _systems)
    {
        if (sys)
            sys(*this);
    }
}
