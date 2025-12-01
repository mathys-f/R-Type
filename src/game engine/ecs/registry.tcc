#pragma once

#include "registry.h"

template <class Component>
SparseArray<Component> &Registry::register_component()
{
    const auto key = std::type_index(typeid(Component));
    auto it = _components_arrays.find(key);

    if (it == _components_arrays.end())
    {
        // create storage for this component type
        auto [new_it, inserted] = _components_arrays.emplace(
            key,
            std::any(SparseArray<Component>{}));

        // register erase callback for this component type
        _erase_functions.emplace_back(
            [](Registry &reg, entity_t const &e)
            {
                auto &arr = reg.get_components<Component>();
                arr.erase(static_cast<typename SparseArray<Component>::size_type>(
                    static_cast<Entity::IdType>(e)));
            });

        return std::any_cast<SparseArray<Component> &>(new_it->second);
    }

    return std::any_cast<SparseArray<Component> &>(it->second);
}

template <class Component>
SparseArray<Component> &Registry::get_components()
{
    const auto key = std::type_index(typeid(Component));
    auto it = _components_arrays.find(key);
    if (it == _components_arrays.end())
        throw std::runtime_error("Component not registered");

    return std::any_cast<SparseArray<Component> &>(it->second);
}

template <class Component>
SparseArray<Component> const &Registry::get_components() const
{
    const auto key = std::type_index(typeid(Component));
    auto it = _components_arrays.find(key);
    if (it == _components_arrays.end())
        throw std::runtime_error("Component not registered");

    return std::any_cast<SparseArray<Component> const &>(it->second);
}

template <typename Component>
typename SparseArray<Component>::reference_type
Registry::add_component(entity_t const &to, Component &&c)
{
    register_component<Component>();
    auto &arr = get_components<Component>();
    auto idx = static_cast<typename SparseArray<Component>::size_type>(
        static_cast<Entity::IdType>(to));
    return arr.insert_at(idx, std::forward<Component>(c));
}

template <typename Component, typename... Params>
typename SparseArray<Component>::reference_type
Registry::emplace_component(entity_t const &to, Params &&...p)
{
    register_component<Component>();
    auto &arr = get_components<Component>();
    auto idx = static_cast<typename SparseArray<Component>::size_type>(
        static_cast<Entity::IdType>(to));
    return arr.emplace_at(idx, std::forward<Params>(p)...);
}

template <typename Component>
void Registry::remove_component(entity_t const &from)
{
    auto &arr = get_components<Component>();
    auto idx = static_cast<typename SparseArray<Component>::size_type>(
        static_cast<Entity::IdType>(from));
    arr.erase(idx);
}

template <class... Components, typename Function>
void Registry::add_system(Function &&f)
{
    // ensure component arrays exist
    (void)std::initializer_list<int>{(register_component<Components>(), 0)...};

    // wrapper that fetches the proper sparse arrays and forwards them
    auto wrapper = [fn = std::forward<Function>(f)](Registry &reg) mutable
    {
        fn(reg, std::as_const(reg.get_components<Components>())...);
    };

    _systems.emplace_back(std::move(wrapper));
}

template <class... Components, typename Function>
void Registry::add_system(Function const &f)
{
    (void)std::initializer_list<int>{(register_component<Components>(), 0)...};

    auto wrapper = [&f](Registry &reg)
    {
        f(reg, std::as_const(reg.get_components<Components>())...);
    };

    _systems.emplace_back(wrapper);
}
