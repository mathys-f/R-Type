#include <gtest/gtest.h>
#include "ecs/registry.h"

TEST(RegistryEntity, SpawnEntity) {
    ecs::Registry registry;
    ecs::Entity e1 = registry.spawn_entity();
    ecs::Entity e2 = registry.spawn_entity();

    EXPECT_NE(e1, e2);
    EXPECT_EQ(e1.value(), 0);
    EXPECT_EQ(e2.value(), 1);
}

TEST(RegistryEntity, KillAndRecycleEntity) {
    ecs::Registry registry;
    ecs::Entity e1 = registry.spawn_entity();
    ecs::Entity e2 = registry.spawn_entity();

    registry.kill_entity(e1);
    ecs::Entity e3 = registry.spawn_entity();

    // e3 should reuse e1's ID (0)
    EXPECT_EQ(e3.value(), 0);
    EXPECT_NE(e3, e2);

    ecs::Entity e4 = registry.spawn_entity();
    EXPECT_EQ(e4.value(), 2);
}

TEST(RegistryEntity, EntityFromIndex) {
    ecs::Registry registry;
    ecs::Entity e = registry.entity_from_index(42);
    EXPECT_EQ(e.value(), 42);
}
