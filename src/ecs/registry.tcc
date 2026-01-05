#pragma once

#include "registry.h"

namespace ecs {

/// Ensure storage exists for `Component` and return a reference to it.
/// @tparam TComponent The component type to register/access.
/// @return Reference to the corresponding `SparseArray<TComponent>`.
template <class TComponent> SparseArray<TComponent>& Registry::register_component() {
    const auto k_key = std::type_index(typeid(TComponent));
    auto it = m_components_arrays.find(k_key);

    if (it == m_components_arrays.end()) {
        auto [new_it, inserted] = m_components_arrays.emplace(k_key, std::any(SparseArray<TComponent>{}));

        m_erase_functions.emplace_back([](Registry& reg, EntityType const& e) {
            auto& arr = reg.get_components<TComponent>();
            arr.erase(static_cast<typename SparseArray<TComponent>::SizeType>(static_cast<Entity::IdType>(e)));
        });

        return std::any_cast<SparseArray<TComponent>&>(new_it->second);
    }

    return std::any_cast<SparseArray<TComponent>&>(it->second);
}

/// Access the non-const component storage for `TComponent`.
/// @tparam TComponent The component type to access.
/// @throws std::runtime_error if the component type is not registered.
template <class TComponent> SparseArray<TComponent>& Registry::get_components() {
    const auto k_key = std::type_index(typeid(TComponent));
    auto it = m_components_arrays.find(k_key);
    if (it == m_components_arrays.end())
        throw std::runtime_error("Component not registered");

    return std::any_cast<SparseArray<TComponent>&>(it->second);
}

/// Access the const component storage for `TComponent`.
/// @tparam TComponent The component type to access.
/// @throws std::runtime_error if the component type is not registered.
template <class TComponent> SparseArray<TComponent> const& Registry::get_components() const {
    const auto k_key = std::type_index(typeid(TComponent));
    auto it = m_components_arrays.find(k_key);
    if (it == m_components_arrays.end())
        throw std::runtime_error("Component not registered");

    return std::any_cast<SparseArray<TComponent> const&>(it->second);
}

/// Add or replace a component instance for the given entity.
/// @tparam TComponent Component type to add.
/// @param to Target entity receiving the component.
/// @param c The component instance (moved or copied).
/// @return Reference to the inserted component slot.
template <typename TComponent>
typename SparseArray<TComponent>::ReferenceType Registry::add_component(EntityType const& to, TComponent&& c) {
    register_component<TComponent>();
    auto& arr = get_components<TComponent>();
    auto idx = static_cast<typename SparseArray<TComponent>::SizeType>(static_cast<Entity::IdType>(to));
    return arr.insert_at(idx, std::forward<TComponent>(c));
}

/// Emplace-construct a component for the entity in-place.
/// @tparam TComponent Component type to emplace.
/// @tparam TParams Constructor parameters forwarded to `TComponent`.
/// @param to Target entity receiving the component.
template <typename TComponent, typename... TParams>
typename SparseArray<TComponent>::ReferenceType Registry::emplace_component(EntityType const& to, TParams&&... p) {
    register_component<TComponent>();
    auto& arr = get_components<TComponent>();
    auto idx = static_cast<typename SparseArray<TComponent>::SizeType>(static_cast<Entity::IdType>(to));
    return arr.emplace_at(idx, std::forward<TParams>(p)...);
}

/// Remove the component of type `TComponent` from an entity.
/// @tparam TComponent Component type to remove.
/// @param from Entity from which the component will be removed.
template <typename TComponent> void Registry::remove_component(EntityType const& from) {
    auto& arr = get_components<TComponent>();
    auto idx = static_cast<typename SparseArray<TComponent>::SizeType>(static_cast<Entity::IdType>(from));
    arr.erase(idx);
}

} // namespace ecs
