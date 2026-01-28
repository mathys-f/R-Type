#include <gtest/gtest.h>
#include "ecs/zipper.h"
#include <vector>
#include <list>

TEST(ZipperTest, ZipTwoVectors) {
    std::vector<int> v1 = {1, 2, 3};
    std::vector<float> v2 = {1.1f, 2.2f, 3.3f};

    auto zip = ecs::zipper(v1, v2);
    int i = 0;
    for (auto [a, b] : zip) {
        EXPECT_EQ(a, v1[i]);
        EXPECT_FLOAT_EQ(b, v2[i]);
        i++;
    }
    EXPECT_EQ(i, 3);
}

TEST(ZipperTest, ZipDifferentSizes) {
    std::vector<int> v1 = {1, 2, 3, 4};
    std::vector<int> v2 = {10, 20};

    auto zip = ecs::zipper(v1, v2);
    int count = 0;
    for (auto [a, b] : zip) {
        EXPECT_EQ(a, v1[count]);
        EXPECT_EQ(b, v2[count]);
        count++;
    }
    // Should iterate up to the smallest container size
    EXPECT_EQ(count, 2);
}

TEST(ZipperTest, ModifyElements) {
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = {10, 20, 30};

    auto zip = ecs::zipper(v1, v2);
    for (auto [a, b] : zip) {
        a += 1;
        b += 1;
    }

    EXPECT_EQ(v1[0], 2);
    EXPECT_EQ(v1[1], 3);
    EXPECT_EQ(v1[2], 4);
    EXPECT_EQ(v2[0], 11);
    EXPECT_EQ(v2[1], 21);
    EXPECT_EQ(v2[2], 31);
}

TEST(ZipperTest, IndexedZipper) {
    std::vector<char> v1 = {'a', 'b', 'c'};
    std::vector<int> v2 = {100, 200, 300};

    auto zip = ecs::indexed_zipper(v1, v2);
    size_t expected_idx = 0;
    for (auto [idx, a, b] : zip) {
        EXPECT_EQ(idx, expected_idx);
        EXPECT_EQ(a, v1[expected_idx]);
        EXPECT_EQ(b, v2[expected_idx]);
        expected_idx++;
    }
    EXPECT_EQ(expected_idx, 3);
}

TEST(ZipperTest, IndexedZipperDifferentSizes) {
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = {10};

    auto zip = ecs::indexed_zipper(v1, v2);
    size_t count = 0;
    for (auto [idx, a, b] : zip) {
        EXPECT_EQ(idx, 0);
        EXPECT_EQ(a, 1);
        EXPECT_EQ(b, 10);
        count++;
    }
    EXPECT_EQ(count, 1);
}

TEST(ZipperTest, IteratorOperations) {
    std::vector<int> v1 = {1, 2, 3};
    auto zip = ecs::zipper(v1);
    
    auto it = zip.begin();
    auto end = zip.end();
    
    EXPECT_NE(it, end);
    EXPECT_EQ(std::get<0>(*it), 1);
    
    // Post-increment
    auto it_copy = it++;
    EXPECT_EQ(std::get<0>(*it_copy), 1);
    EXPECT_EQ(std::get<0>(*it), 2);
    
    // Pre-increment
    ++it;
    EXPECT_EQ(std::get<0>(*it), 3);
    
    ++it;
    EXPECT_EQ(it, end);
}

TEST(ZipperTest, IndexedIteratorOperations) {
    std::vector<int> v1 = {10, 20};
    auto zip = ecs::indexed_zipper(v1);
    
    auto it = zip.begin();
    auto end = zip.end();
    
    EXPECT_NE(it, end);
    auto [idx1, val1] = *it;
    EXPECT_EQ(idx1, 0);
    EXPECT_EQ(val1, 10);
    
    it++;
    auto [idx2, val2] = *it;
    EXPECT_EQ(idx2, 1);
    EXPECT_EQ(val2, 20);
    
    ++it;
    EXPECT_EQ(it, end);
}
