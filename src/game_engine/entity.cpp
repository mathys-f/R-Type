#include "entity.hpp"

Entity::operator Entity::IdType() const noexcept
{
    return _id;
}

Entity::IdType Entity::value() const noexcept
{
    return _id;
}

Entity::Entity(IdType v) noexcept : _id(v)
{
}
