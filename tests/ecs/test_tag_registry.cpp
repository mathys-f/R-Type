#include <gtest/gtest.h>
#include "ecs/tag_registry.h"
#include "ecs/entity.h"

// We need to bypass the private constructor of Entity or use a friend, 
// but for testing TagRegistry we can just cast, 
// OR we can use the Registry to create entities if we wanted integration test.
// TagRegistry uses Entity just as a value. 
// However, Entity constructor is private. 
// We should probably test TagRegistry via Registry or just hack the Entity creation.
// Actually Registry::entity_from_index is public and creates an Entity.

#include "ecs/registry.h"

TEST(TagRegistry, CreateAndBindTag) {
    ecs::Registry registry;
    ecs::TagRegistry& tags = registry.get_tag_registry();
    ecs::Entity e = registry.spawn_entity();

    auto tagId = tags.create_tag("Player");
    tags.bind_tag(tagId, e);

    EXPECT_EQ(tags.get_tag_id("Player"), tagId);
    EXPECT_EQ(tags.get_tag_id(e), tagId);
    EXPECT_EQ(tags.get_tag_name(tagId), "Player");
    
    auto retrievedEntity = tags.get_entity("Player");
    ASSERT_TRUE(retrievedEntity.has_value());
    EXPECT_EQ(retrievedEntity.value(), e);
}

TEST(TagRegistry, CreateAndBindTagCombined) {
    ecs::Registry registry;
    ecs::TagRegistry& tags = registry.get_tag_registry();
    ecs::Entity e = registry.spawn_entity();

    auto tagId = tags.create_and_bind_tag("Enemy", e);

    EXPECT_EQ(tags.get_tag_name(e), "Enemy");
    EXPECT_EQ(tags.get_entity(tagId).value(), e);
}

TEST(TagRegistry, NonExistentTag) {
    ecs::Registry registry;
    ecs::TagRegistry& tags = registry.get_tag_registry();

    EXPECT_EQ(tags.get_tag_id("NonExistent"), ecs::TagRegistry::k_invalid_tag_id);
    EXPECT_FALSE(tags.get_entity("NonExistent").has_value());
}
