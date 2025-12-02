#pragma once

#include "entity.h"
#include "sparse_array.h"

#include <any>
#include <functional>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace ecs
{

/// Registry manages entities, component storage and registered systems.
///
/// - Provides component registration and type-erased storage for component
///   arrays (SparseArray).
/// - Manages entity creation / recycling.
/// - Allows systems to be registered and executed with bound component views.
class Registry
{
  public:
    using EntityType = Entity;

    // Component registration / access
    /// Register storage for a component type if not already present.
    /// @tparam Component The component type to register.
    /// @return A reference to the underlying SparseArray for `Component`.
    template <class Component> SparseArray<Component>& register_component();

    /// Get the component storage for `Component` (non-const).
    /// @tparam Component The component type to access.
    /// @return Non-const reference to the SparseArray holding `Component`.
    template <class Component> SparseArray<Component>& get_components();

    /// Get the component storage for `Component` (const).
    /// @tparam Component The component type to access.
    /// @return Const reference to the SparseArray holding `Component`.
    template <class Component> SparseArray<Component> const& get_components() const;

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
    /// @tparam Component Component type to add.
    /// @param to Target entity receiving the component.
    /// @param c The component instance to add (moved or copied).
    /// @return A reference to the inserted component storage slot.
    template <typename Component>
    typename SparseArray<Component>::ReferenceType add_component(EntityType const& to,
                                                                 Component&& c);

    /// Emplace-construct a component for the entity in-place.
    /// @tparam Component Component type to emplace.
    /// @tparam Params Constructor parameter pack forwarded to Component.
    /// @param to Target entity receiving the component.
    /// @param p Parameters forwarded to Component's constructor.
    /// @return A reference to the emplaced component storage slot.
    template <typename Component, typename... Params>
    typename SparseArray<Component>::ReferenceType emplace_component(EntityType const& to,
                                                                     Params&&... p);

    /// Remove the component of type `Component` from an entity.
    /// @tparam Component Component type to remove.
    /// @param from Entity from which the component will be removed.
    template <typename Component> void remove_component(EntityType const& from);

    // System registration / execution
    /// Register a system that accepts const views to the requested
    /// component storages. The callable should accept `(Registry&, const
    /// SparseArray<...>&...)` for the listed component types.
    /// @tparam Components Component types the system requires.
    /// @tparam Function Callable type.
    /// @param f The function object to register.
    template <class... Components, typename Function> void add_system(Function&& f);

    /// Register a const-qualified callable system.
    template <class... Components, typename Function> void add_system(Function const& f);

    /// Execute all registered systems in order.
    void run_systems();

  private:
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

}  // namespace ecs

#include "registry.tcc"
