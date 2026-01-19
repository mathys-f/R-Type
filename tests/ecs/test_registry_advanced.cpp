#include <gtest/gtest.h>
#include "ecs/registry.h"

struct TestComp {
    int val;
    bool operator==(const TestComp& other) const { return val == other.val; }
};

TEST(RegistryAdvanced, VersioningAndTombstones) {
    ecs::Registry registry;
    registry.set_current_version(10);
    EXPECT_EQ(registry.get_current_version(), 10);

    ecs::Entity e = registry.spawn_entity();
    
    // Check creation tombstone
    const auto& creation_tombstones = registry.get_entity_creation_tombstones();
    auto it = creation_tombstones.find(e);
    ASSERT_NE(it, creation_tombstones.end());
    EXPECT_EQ(it->second, 10);
    
    registry.remove_entity_creation_tombstone(e);
    EXPECT_EQ(registry.get_entity_creation_tombstones().size(), 0);

    // Check destruction tombstone
    registry.kill_entity(e);
    const auto& destruction_tombstones = registry.get_entity_destruction_tombstones();
    auto it_dest = destruction_tombstones.find(e);
    ASSERT_NE(it_dest, destruction_tombstones.end());
    EXPECT_EQ(it_dest->second, 10);
    
    registry.remove_entity_destruction_tombstone(e);
    EXPECT_EQ(registry.get_entity_destruction_tombstones().size(), 0);
}

TEST(RegistryAdvanced, ComponentTombstonesAndMetadata) {
    ecs::Registry registry;
    registry.set_current_version(5);
    
    ecs::Entity e = registry.spawn_entity();
    registry.register_component<TestComp>();
    registry.add_component(e, TestComp{42});
    
    // Check component metadata (dirty/update tracking)
    const auto& metadata = registry.get_component_metadata();
    auto type_idx = std::type_index(typeid(TestComp));
    auto it_meta = metadata.find({e, type_idx});
    ASSERT_NE(it_meta, metadata.end());
    EXPECT_EQ(it_meta->second, 5);

    // Manually mark dirty
    registry.set_current_version(6);
    registry.mark_dirty<TestComp>(e);
    it_meta = metadata.find({e, type_idx});
    EXPECT_EQ(it_meta->second, 6);

    // Remove component and check destruction tombstone
    registry.remove_component<TestComp>(e);
    
    const auto& comp_dest_tombstones = registry.get_component_destruction_tombstones();
    ASSERT_NE(comp_dest_tombstones.find(e), comp_dest_tombstones.end());
    auto& inner_map = comp_dest_tombstones.at(e);
    ASSERT_NE(inner_map.find(type_idx), inner_map.end());
    EXPECT_EQ(inner_map.at(type_idx), 6);
    
    // Clean up
    registry.remove_component_destruction_tombstone(e, type_idx);
    if (comp_dest_tombstones.find(e) != comp_dest_tombstones.end()) {
         // It might still exist but be empty, or not fully erased depending on implementation logic
         // The test just checks that specific entry is gone or map is updated
         auto& inner = comp_dest_tombstones.at(e);
         EXPECT_EQ(inner.find(type_idx), inner.end());
    }
    
    registry.remove_component_metadata(e, type_idx);
    EXPECT_EQ(registry.get_component_metadata().find({e, type_idx}), registry.get_component_metadata().end());
}

TEST(RegistryAdvanced, GetEntityComponents) {
    ecs::Registry registry;
    ecs::Entity e = registry.spawn_entity();
    
    registry.register_component<TestComp>();
    registry.add_component(e, TestComp{123});
    
    const auto& components = registry.get_entity_components(e);
    auto type_idx = std::type_index(typeid(TestComp));
    
    ASSERT_NE(components.find(type_idx), components.end());
    const std::any& any_val = components.at(type_idx);
    EXPECT_EQ(std::any_cast<TestComp>(any_val).val, 123);
}
