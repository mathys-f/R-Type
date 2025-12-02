#include "entity.h"

Entity::operator Entity::IdType() const noexcept
{
    return m_id;
}

Entity::IdType Entity::value() const noexcept
{
    return m_id;
}

Entity::Entity(IdType v) noexcept : m_id(v)
{
}
