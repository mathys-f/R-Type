#pragma once

#include <cstddef>

class Registry;

/// Strongly-typed entity ID wrapper.
/// Explicit construction from size_t, implicit conversion to size_t.
/// Only Registry can construct arbitrary IDs.
class Entity
{
  public:
    using IdType = std::size_t;

    Entity() noexcept = default;

    // implicit conversion to the underlying index
    operator IdType() const noexcept;

    IdType value() const noexcept;

  private:
    explicit Entity(IdType v) noexcept;

    IdType _id{0};

    friend class Registry;
};
