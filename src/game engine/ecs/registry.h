#pragma once

#include "sparse_array.h"
#include "../entity.hpp"

#include <any>
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <stdexcept>

class Registry
{
public:
    using entity_t = Entity;

    // Component registration / access
    template <class Component>
    SparseArray<Component> &register_component();

    template <class Component>
    SparseArray<Component> &get_components();

    template <class Component>
    SparseArray<Component> const &get_components() const;

    // Entity lifecycle
    entity_t spawn_entity();
    entity_t entity_from_index(std::size_t idx) const noexcept;
    void kill_entity(entity_t const &e);

    // Component management
    template <typename Component>
    typename SparseArray<Component>::reference_type
    add_component(entity_t const &to, Component &&c);

    template <typename Component, typename... Params>
    typename SparseArray<Component>::reference_type
    emplace_component(entity_t const &to, Params &&...p);

    template <typename Component>
    void remove_component(entity_t const &from);

    // System registration / execution (Step 4)
    template <class... Components, typename Function>
    void add_system(Function &&f);

    template <class... Components, typename Function>
    void add_system(Function const &f);

    void run_systems();

private:
    // one sparse array per component type, stored via type erasure
    std::unordered_map<std::type_index, std::any> _components_arrays;

    // erase callbacks (one per component type)
    std::vector<std::function<void(Registry &, entity_t const &)>> _erase_functions;

    // registered systems (Step 4)
    std::vector<std::function<void(Registry &)>> _systems;

    // entity id management
    Entity::IdType _next_entity{0};
    std::vector<entity_t> _free_entities;
};

#include "registry.tcc"
