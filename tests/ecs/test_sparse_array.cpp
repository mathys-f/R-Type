#include <gtest/gtest.h>
#include "ecs/sparse_array.h"

struct TestComponent {
    int value;
    bool operator==(const TestComponent& other) const { return value == other.value; }
};

TEST(SparseArray, InsertAt) {
    ecs::SparseArray<TestComponent> array;
    
    array.insert_at(0, TestComponent{10});
    array.insert_at(2, TestComponent{20});

    EXPECT_EQ(array.size(), 3);
    EXPECT_TRUE(array[0].has_value());
    EXPECT_EQ(array[0]->value, 10);
    EXPECT_FALSE(array[1].has_value());
    EXPECT_TRUE(array[2].has_value());
    EXPECT_EQ(array[2]->value, 20);
}

TEST(SparseArray, EmplaceAt) {
    ecs::SparseArray<TestComponent> array;

    array.emplace_at(1, 30);
    
    EXPECT_EQ(array.size(), 2);
    EXPECT_FALSE(array[0].has_value());
    EXPECT_TRUE(array[1].has_value());
    EXPECT_EQ(array[1]->value, 30);
}

TEST(SparseArray, Erase) {
    ecs::SparseArray<TestComponent> array;
    
    array.insert_at(0, TestComponent{10});
    EXPECT_TRUE(array[0].has_value());

    array.erase(0);
    EXPECT_FALSE(array[0].has_value());
    EXPECT_EQ(array.size(), 1); // Size doesn't shrink on erase
}

TEST(SparseArray, GetIndex) {
    ecs::SparseArray<TestComponent> array;
    
    array.insert_at(0, TestComponent{10});
    array.insert_at(5, TestComponent{50});

    auto& ref0 = array[0];
    auto& ref5 = array[5];

    EXPECT_EQ(array.get_index(ref0), 0);
    EXPECT_EQ(array.get_index(ref5), 5);
}

TEST(SparseArray, InsertCopy) {
    ecs::SparseArray<TestComponent> array;
    TestComponent c{100};
    
    array.insert_at(2, c);
    
    EXPECT_TRUE(array[2].has_value());
    EXPECT_EQ(array[2]->value, 100);
}

TEST(SparseArray, Iterators) {
    ecs::SparseArray<TestComponent> array;
    array.insert_at(0, TestComponent{1});
    array.insert_at(2, TestComponent{3});

    auto it = array.begin();
    EXPECT_NE(it, array.end());
    EXPECT_TRUE(it->has_value());
    EXPECT_EQ(it->value().value, 1);
    
    it++; // index 1 (empty)
    EXPECT_FALSE(it->has_value());
    
    it++; // index 2
    EXPECT_TRUE(it->has_value());
    EXPECT_EQ(it->value().value, 3);
    
    it++;
    EXPECT_EQ(it, array.end());
}

TEST(SparseArray, ConstAccess) {
    ecs::SparseArray<TestComponent> array;
    array.insert_at(0, TestComponent{10});
    
    const auto& const_array = array;
    
    EXPECT_TRUE(const_array[0].has_value());
    EXPECT_EQ(const_array[0]->value, 10);
    
    auto it = const_array.begin();
    EXPECT_NE(it, const_array.end());
    
    auto cit = const_array.cbegin();
    EXPECT_NE(cit, const_array.cend());
    EXPECT_EQ(cit->value().value, 10);
}
