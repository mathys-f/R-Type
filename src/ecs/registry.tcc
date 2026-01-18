#pragma once

#include "registry.h"

namespace ecs {

/// Set current version (Used to track changes).
/// @param v The new version to set.
template <typename TComponent> inline void Registry::mark_dirty(EntityType const& e) {
    m_component_metadata[{e, std::type_index(typeid(TComponent))}] = m_current_version;
}

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

        // Add extraction function for get_entity_components
        m_extract_functions[k_key] = ([k_key](const Registry& reg, EntityType const& e) -> std::optional<std::any> {
            try {
                const auto& arr = reg.get_components<TComponent>();
                auto idx = static_cast<typename SparseArray<TComponent>::SizeType>(static_cast<Entity::IdType>(e));
                if (idx >= arr.size() || !arr[idx].has_value()) {
                    return std::nullopt;
                }
                return std::make_any<TComponent>(arr[idx].value());
            } catch (...) {
                return std::nullopt;
            }
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

    m_component_metadata[{to, std::type_index(typeid(TComponent))}] = m_current_version;

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

    m_component_metadata[{to, std::type_index(typeid(TComponent))}] = m_current_version;

    return arr.emplace_at(idx, std::forward<TParams>(p)...);
}

/// Remove the component of type `TComponent` from an entity.
/// @tparam TComponent Component type to remove.
/// @param from Entity from which the component will be removed.
template <typename TComponent> void Registry::remove_component(EntityType const& from) {
    auto& arr = get_components<TComponent>();
    auto idx = static_cast<typename SparseArray<TComponent>::SizeType>(static_cast<Entity::IdType>(from));

    m_component_destruction_tombstones[from][std::type_index(typeid(TComponent))] = m_current_version;

    arr.erase(idx);
}

template <typename TComponent> inline bool Registry::has_component(EntityType const& entity) const {
    const auto& arr = get_components<TComponent>();
    auto idx = static_cast<typename SparseArray<TComponent>::SizeType>(static_cast<Entity::IdType>(entity));
    if (idx >= arr.size())
        return false;
    return arr[idx].has_value();
}

} // namespace ecs
