#pragma once

#include <cstddef>
#include <format>
#include <functional>

namespace ecs {

class Registry;

/// Strongly-typed wrapper for entity identifiers used by the ECS.
/// Provides an explicit IdType alias and safe conversions to the underlying
/// integral identifier.
class Entity {
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

    /// Equality comparison operator.
    /// @param other The other entity to compare with.
    /// @return True if the entities have the same id.
    bool operator==(const Entity& other) const noexcept {
        return m_id == other.m_id;
    }

    /// Inequality comparison operator.
    /// @param other The other entity to compare with.
    /// @return True if the entities have different ids.
    bool operator!=(const Entity& other) const noexcept {
        return m_id != other.m_id;
    }

  private:
    /// Construct an Entity from a raw id. Only `Registry` may create entities.
    /// @param v The raw id to wrap.
    explicit Entity(IdType v) noexcept;

    /// The underlying id stored for this entity.
    IdType m_id{0};

    /// `Registry` is a friend to allow it to construct and manage entities.
    friend class Registry;
};

} // namespace ecs

// Hash specialization for Entity to make it usable in std::unordered_map
template <>
struct std::hash<ecs::Entity> {
    std::size_t operator()(const ecs::Entity& entity) const noexcept {
        return std::hash<ecs::Entity::IdType>{}(entity.value());
    }
};

template <> struct std::formatter<ecs::Entity> : std::formatter<size_t> {
    auto format(const ecs::Entity& entity, auto& ctx) const {
        return std::formatter<size_t>::format(static_cast<size_t>(entity), ctx);
    }
};
