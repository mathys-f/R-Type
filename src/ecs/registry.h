#pragma once

#include "entity.h"
#include "sparse_array.h"
#include "tag_registry.h"

#include <any>
#include <functional>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace ecs {

/// Registry manages entities, component storage and registered systems.
///
/// - Provides component registration and type-erased storage for component
///   arrays (SparseArray).
/// - Manages entity creation / recycling.
/// - Allows systems to be registered and executed with bound component views.
class Registry {
  public:
    using EntityType = Entity;

    // Component registration / access
    /// Register storage for a component type if not already present.
    /// @tparam TComponent The component type to register.
    /// @return A reference to the underlying SparseArray for `TComponent`.
    template <class TComponent> SparseArray<TComponent>& register_component();

    /// Get the component storage for `TComponent` (non-const).
    /// @tparam TComponent The component type to access.
    /// @return Non-const reference to the SparseArray holding `TComponent`.
    template <class TComponent> SparseArray<TComponent>& get_components();

    /// Get the component storage for `TComponent` (const).
    /// @tparam TComponent The component type to access.
    /// @return Const reference to the SparseArray holding `TComponent`.
    template <class TComponent> SparseArray<TComponent> const& get_components() const;

    // Entity lifecycle
    /// Create a new entity id, reusing freed ids when possible.
    /// @return A new `Entity` handle.
    EntityType spawn_entity();

    /// Convert a raw index to an `Entity` object.
    /// @param idx The raw index value.
    /// @return An `Entity` corresponding to the index.
    EntityType entity_from_index(std::size_t idx) const noexcept;

    /// Mark an entity as dead and remove its components.
    /// @param e The entity to destroy.
    void kill_entity(EntityType const& e);

    // Component management
    /// Add or replace a component instance for the given entity.
    /// @tparam TComponent Component type to add.
    /// @param to Target entity receiving the component.
    /// @param c The component instance to add (moved or copied).
    /// @return A reference to the inserted component storage slot.
    template <typename TComponent>
    typename SparseArray<TComponent>::ReferenceType add_component(EntityType const& to, TComponent&& c);

    /// Emplace-construct a component for the entity in-place.
    /// @tparam TComponent Component type to emplace.
    /// @tparam TParams Constructor parameter pack forwarded to TComponent.
    /// @param to Target entity receiving the component.
    /// @param p Parameters forwarded to TComponent's constructor.
    /// @return A reference to the emplaced component storage slot.
    template <typename TComponent, typename... TParams>
    typename SparseArray<TComponent>::ReferenceType emplace_component(EntityType const& to, TParams&&... p);

    /// Remove the component of type `TComponent` from an entity.
    /// @tparam TComponent Component type to remove.
    /// @param from Entity from which the component will be removed.
    template <typename TComponent> void remove_component(EntityType const& from);

    const std::unordered_map<std::type_index, std::any>& dump_components() const noexcept;

    const std::unordered_map<std::type_index, std::any>& dump_entity_components(const Entity& e) const noexcept;

    /// Get the tag registry (non-const).
    /// @return Reference to the tag registry.
    TagRegistry& get_tag_registry() noexcept;

    /// Get the tag registry (const).
    /// @return Const reference to the tag registry.
    const TagRegistry& get_tag_registry() const noexcept;

  private:
    // Tag registry
    TagRegistry tag_registry;

    // one sparse array per component type, stored via type erasure
    std::unordered_map<std::type_index, std::any> m_components_arrays;

    // erase callbacks (one per component type)
    std::vector<std::function<void(Registry&, EntityType const&)>> m_erase_functions;

    // registered systems
    std::vector<std::function<void(Registry&)>> m_systems;

    // entity id management
    Entity::IdType m_next_entity{0};
    std::vector<EntityType> m_free_entities;
};

} // namespace ecs

#include "registry.tcc"
