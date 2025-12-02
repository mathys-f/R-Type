#pragma once

#include "registry.h"

/// Ensure storage exists for `Component` and return a reference to it.
/// @tparam Component The component type to register/access.
/// @return Reference to the corresponding `SparseArray<Component>`.
template <class Component> SparseArray<Component>& Registry::register_component()
{
    const auto key = std::type_index(typeid(Component));
    auto it = m_components_arrays.find(key);

    if (it == m_components_arrays.end())
    {
        auto [new_it, inserted] =
            m_components_arrays.emplace(key, std::any(SparseArray<Component>{}));

        m_erase_functions.emplace_back(
            [](Registry& reg, EntityType const& e)
            {
                auto& arr = reg.get_components<Component>();
                arr.erase(static_cast<typename SparseArray<Component>::SizeType>(
                    static_cast<Entity::IdType>(e)));
            });

        return std::any_cast<SparseArray<Component>&>(new_it->second);
    }

    return std::any_cast<SparseArray<Component>&>(it->second);
}

/// Access the non-const component storage for `Component`.
/// @tparam Component The component type to access.
/// @throws std::runtime_error if the component type is not registered.
template <class Component> SparseArray<Component>& Registry::get_components()
{
    const auto key = std::type_index(typeid(Component));
    auto it = m_components_arrays.find(key);
    if (it == m_components_arrays.end())
        throw std::runtime_error("Component not registered");

    return std::any_cast<SparseArray<Component>&>(it->second);
}

/// Access the const component storage for `Component`.
/// @tparam Component The component type to access.
/// @throws std::runtime_error if the component type is not registered.
template <class Component> SparseArray<Component> const& Registry::get_components() const
{
    const auto key = std::type_index(typeid(Component));
    auto it = m_components_arrays.find(key);
    if (it == m_components_arrays.end())
        throw std::runtime_error("Component not registered");

    return std::any_cast<SparseArray<Component> const&>(it->second);
}

/// Add or replace a component instance for the given entity.
/// @tparam Component Component type to add.
/// @param to Target entity receiving the component.
/// @param c The component instance (moved or copied).
/// @return Reference to the inserted component slot.
template <typename Component>
typename SparseArray<Component>::ReferenceType Registry::add_component(EntityType const& to,
                                                                       Component&& c)
{
    register_component<Component>();
    auto& arr = get_components<Component>();
    auto idx =
        static_cast<typename SparseArray<Component>::SizeType>(static_cast<Entity::IdType>(to));
    return arr.insert_at(idx, std::forward<Component>(c));
}

/// Emplace-construct a component for the entity in-place.
/// @tparam Component Component type to emplace.
/// @tparam Params Constructor parameters forwarded to `Component`.
/// @param to Target entity receiving the component.
template <typename Component, typename... Params>
typename SparseArray<Component>::ReferenceType Registry::emplace_component(EntityType const& to,
                                                                           Params&&... p)
{
    register_component<Component>();
    auto& arr = get_components<Component>();
    auto idx =
        static_cast<typename SparseArray<Component>::SizeType>(static_cast<Entity::IdType>(to));
    return arr.emplace_at(idx, std::forward<Params>(p)...);
}

/// Remove the component of type `Component` from an entity.
/// @tparam Component Component type to remove.
/// @param from Entity from which the component will be removed.
template <typename Component> void Registry::remove_component(EntityType const& from)
{
    auto& arr = get_components<Component>();
    auto idx =
        static_cast<typename SparseArray<Component>::SizeType>(static_cast<Entity::IdType>(from));
    arr.erase(idx);
}

/// Register a system that will be executed later via `run_systems`.
/// The callable is wrapped to fetch const references to the requested
/// component storages and forward them to the provided function.
template <class... Components, typename Function> void Registry::add_system(Function&& f)
{
    (void)std::initializer_list<int>{(register_component<Components>(), 0)...};

    auto wrapper = [fn = std::forward<Function>(f)](Registry& reg) mutable
    { fn(reg, std::as_const(reg.get_components<Components>())...); };

    m_systems.emplace_back(std::move(wrapper));
}

/// Same as the rvalue overload but accepts a const-qualified callable.
template <class... Components, typename Function> void Registry::add_system(Function const& f)
{
    (void)std::initializer_list<int>{(register_component<Components>(), 0)...};

    auto wrapper = [&f](Registry& reg)
    { f(reg, std::as_const(reg.get_components<Components>())...); };

    m_systems.emplace_back(wrapper);
}
