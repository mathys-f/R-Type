#pragma once

#include <cstddef>

class Registry;

/// Strongly-typed wrapper for entity identifiers used by the ECS.
/// Provides an explicit IdType alias and safe conversions to the underlying
/// integral identifier.
class Entity
{
  public:
    /// Underlying integer type used for entity ids.
    using IdType = std::size_t;

    /// Default-construct an invalid / null entity.
    Entity() noexcept = default;

    /// Convert the Entity to its underlying `IdType` value.
    /// @return The raw entity id as `IdType`.
    operator IdType() const noexcept;

    /// Get the underlying id value for this entity.
    /// @return The raw entity id as `IdType`.
    IdType value() const noexcept;

  private:
    /// Construct an Entity from a raw id. Only `Registry` may create entities.
    /// @param v The raw id to wrap.
    explicit Entity(IdType v) noexcept;

    /// The underlying id stored for this entity.
    IdType m_id{0};

    /// `Registry` is a friend to allow it to construct and manage entities.
    friend class Registry;
};
