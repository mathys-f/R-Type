#include <gtest/gtest.h>
#include "ecs/registry.h"

struct Position {
    float x, y;
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

struct Velocity {
    float dx, dy;
};

TEST(RegistryComponent, RegisterComponent) {
    ecs::Registry registry;
    registry.register_component<Position>();
    
    EXPECT_NO_THROW(registry.get_components<Position>());
    EXPECT_THROW(registry.get_components<Velocity>(), std::runtime_error);
}

TEST(RegistryComponent, AddComponent) {
    ecs::Registry registry;
    ecs::Entity e = registry.spawn_entity();

    registry.add_component(e, Position{10.0f, 20.0f});

    EXPECT_TRUE(registry.has_component<Position>(e));
    
    auto& positions = registry.get_components<Position>();
    EXPECT_TRUE(positions[e.value()].has_value());
    EXPECT_EQ(positions[e.value()]->x, 10.0f);
    EXPECT_EQ(positions[e.value()]->y, 20.0f);
}

TEST(RegistryComponent, RemoveComponent) {
    ecs::Registry registry;
    ecs::Entity e = registry.spawn_entity();

    registry.add_component(e, Position{10.0f, 20.0f});
    EXPECT_TRUE(registry.has_component<Position>(e));

    registry.remove_component<Position>(e);
    EXPECT_FALSE(registry.has_component<Position>(e));
}

TEST(RegistryComponent, EmplaceComponent) {
    ecs::Registry registry;
    ecs::Entity e = registry.spawn_entity();

    registry.emplace_component<Position>(e, 30.0f, 40.0f);

    EXPECT_TRUE(registry.has_component<Position>(e));
    auto& positions = registry.get_components<Position>();
    EXPECT_EQ(positions[e.value()]->x, 30.0f);
    EXPECT_EQ(positions[e.value()]->y, 40.0f);
}

TEST(RegistryComponent, KillEntityRemovesComponents) {
    ecs::Registry registry;
    ecs::Entity e = registry.spawn_entity();

    registry.register_component<Position>();
    registry.add_component(e, Position{10.0f, 20.0f});

    EXPECT_TRUE(registry.has_component<Position>(e));

    registry.kill_entity(e);

    // After killing, the component should be removed from the sparse array
    // Note: kill_entity calls the erase functions registered during component registration
    auto& positions = registry.get_components<Position>();
    
    // We need to check if the component at that index is empty
    EXPECT_FALSE(positions[e.value()].has_value());
}
