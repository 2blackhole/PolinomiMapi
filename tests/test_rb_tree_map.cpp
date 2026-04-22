#include <gtest/gtest.h>
#include "RBTreeMap.h"
#include <string>
#include <random>
#include <chrono>
#include <iostream>
#include <map>
#include <vector>

template <typename Map>
bool isBST(Map& map) {
    if (map.empty()) return true;
    auto it = map.begin();
    auto prev = it++;
    while (it != map.end()) {
        if (prev->first >= it->first) return false;
        ++prev; ++it;
    }
    return true;
}

class RBTreeMapTest : public ::testing::Test {
protected:
    RBTreeMap<int, std::string> emptyMap;
    RBTreeMap<int, std::string> map;

    void SetUp() override {
        map.insert({3, "three"});
        map.insert({1, "one"});
        map.insert({4, "four"});
        map.insert({2, "two"});
    }
};


TEST_F(RBTreeMapTest, DefaultConstructedIsEmpty) {
    EXPECT_TRUE(emptyMap.empty());
    EXPECT_EQ(emptyMap.size(), 0);
}

TEST_F(RBTreeMapTest, InsertAndSize) {
    RBTreeMap<int, std::string> m;
    EXPECT_TRUE(m.empty());
    EXPECT_EQ(m.size(), 0);

    auto [it, inserted] = m.insert({5, "five"});
    EXPECT_TRUE(inserted);
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(it->first, 5);
    EXPECT_EQ(it->second, "five");

    auto [it2, inserted2] = m.insert({5, "five again"});
    EXPECT_FALSE(inserted2);
    EXPECT_EQ(m.size(), 1);
    EXPECT_EQ(it2->first, 5);
    EXPECT_EQ(it2->second, "five");
}

TEST_F(RBTreeMapTest, FindExistingKey) {
    auto it = map.find(2);
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->first, 2);
    EXPECT_EQ(it->second, "two");
}

TEST_F(RBTreeMapTest, FindNonExistingKey) {
    auto it = map.find(100);
    EXPECT_EQ(it, map.end());
}

TEST_F(RBTreeMapTest, SubscriptAccessExisting) {
    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[3], "three");
    EXPECT_EQ(map.size(), 4);
}

TEST_F(RBTreeMapTest, SubscriptInsertNew) {
    std::string& val = map[5];
    EXPECT_EQ(val, "");
    EXPECT_EQ(map.size(), 5);
    val = "five";
    EXPECT_EQ(map[5], "five");
}

TEST_F(RBTreeMapTest, SubscriptModifyExisting) {
    map[2] = "two modified";
    EXPECT_EQ(map[2], "two modified");
}

TEST_F(RBTreeMapTest, EraseByKeyExisting) {
    size_t erased = map.erase(2);
    EXPECT_EQ(erased, 1);
    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.find(2), map.end());
}

TEST_F(RBTreeMapTest, EraseByKeyNonExisting) {
    size_t erased = map.erase(100);
    EXPECT_EQ(erased, 0);
    EXPECT_EQ(map.size(), 4);
}

TEST_F(RBTreeMapTest, EraseByIterator) {
    auto it = map.find(3);
    ASSERT_NE(it, map.end());
    auto next = map.erase(it);
    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.find(3), map.end());
    ASSERT_NE(next, map.end());
    EXPECT_EQ(next->first, 4);
}

TEST_F(RBTreeMapTest, EraseByIteratorEnd) {
    auto it = map.find(4);
    auto next = map.erase(it);
    EXPECT_EQ(next, map.end());
    EXPECT_EQ(map.size(), 3);
}

TEST_F(RBTreeMapTest, IterationOrder) {
    std::vector<int> expected = {1, 2, 3, 4};
    std::vector<int> actual;
    for (const auto& p : map) actual.push_back(p.first);
    EXPECT_EQ(actual, expected);
}

TEST_F(RBTreeMapTest, InsertMaintainsOrder) {
    map.insert({0, "zero"});
    std::vector<int> expected = {0, 1, 2, 3, 4};
    std::vector<int> actual;
    for (const auto& p : map) actual.push_back(p.first);
    EXPECT_EQ(actual, expected);
}

TEST_F(RBTreeMapTest, EraseMaintainsOrder) {
    map.erase(2);
    std::vector<int> expected = {1, 3, 4};
    std::vector<int> actual;
    for (const auto& p : map) actual.push_back(p.first);
    EXPECT_EQ(actual, expected);
}

TEST_F(RBTreeMapTest, ModifyViaIterator) {
    auto it = map.find(1);
    ASSERT_NE(it, map.end());
    it->second = "modified";
    EXPECT_EQ(map[1], "modified");
}

TEST_F(RBTreeMapTest, BSTProperty) {
    EXPECT_TRUE(isBST(map));
}

TEST(RBTreeMapStringTest, StringKeys) {
    RBTreeMap<std::string, int> m;
    m.insert({"apple", 10});
    m.insert({"banana", 20});
    m.insert({"cherry", 30});

    EXPECT_EQ(m["banana"], 20);
    EXPECT_EQ(m.find("apple")->second, 10);

    std::vector<std::string> expected = {"apple", "banana", "cherry"};
    std::vector<std::string> keys;
    for (const auto& p : m) keys.push_back(p.first);
    EXPECT_EQ(keys, expected);
}

TEST(RBTreeMapLargeTest, ManyElements) {
    RBTreeMap<int, int> m;
    const int N = 1000;
    for (int i = 0; i < N; ++i) m.insert({i, i * 10});
    EXPECT_EQ(m.size(), N);
    for (int i = 0; i < N; ++i) {
        auto it = m.find(i);
        ASSERT_NE(it, m.end());
        EXPECT_EQ(it->second, i * 10);
    }
    int i = 0;
    for (const auto& p : m) EXPECT_EQ(p.first, i++);
}

TEST(RBTreeMapEraseTest, EraseAndReinsert) {
    RBTreeMap<int, int> m;
    m.insert({1, 10});
    m.erase(1);
    EXPECT_EQ(m.size(), 0);
    EXPECT_EQ(m.find(1), m.end());

    m.insert({1, 20});
    auto it = m.find(1);
    ASSERT_NE(it, m.end());
    EXPECT_EQ(it->second, 20);
}
TEST(RBTreeMapStressTest, ManyInsertEraseBalanced) {
    using Map = RBTreeMap<int, int>;
    Map map;
    std::map<int, int> ref;

    const int N = 100000;       
    const int TARGET_SIZE = 20000;  
    const int OPS = 1000000;

    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> keyDist(1, N);
    std::uniform_int_distribution<int> valDist(1, 1000);
    

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < OPS; ++i) {
        bool doInsert;

        if (map.size() < TARGET_SIZE) {
            doInsert = true;
        }
        else if (map.size() > TARGET_SIZE * 2) {
            doInsert = false;
        }
        else {
            doInsert = rng() % 2;
        }

        if (doInsert) {
            int key = keyDist(rng);
            int val = valDist(rng);

            auto r1 = map.insert({ key, val });
            auto r2 = ref.insert({ key, val });

            EXPECT_EQ(r1.second, r2.second);
        }
        else {
            int key = keyDist(rng);

            size_t e1 = map.erase(key);
            size_t e2 = ref.erase(key);

            EXPECT_EQ(e1, e2);
        }

        if (i % 50000 == 0) {
            EXPECT_EQ(map.size(), ref.size()) << "Step " << i;

            for (const auto& p : ref) {
                auto it = map.find(p.first);
                ASSERT_NE(it, map.end());
                EXPECT_EQ(it->second, p.second);
            }
        }
    }
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "[Stress] RBTree ManyFinds (" << TARGET_SIZE << " operation) done in "
        << elapsed.count() << " sec\n";

    EXPECT_EQ(map.size(), ref.size());

    auto it = map.begin();
    auto rit = ref.begin();

    while (it != map.end() && rit != ref.end()) {
        EXPECT_EQ(it->first, rit->first);
        EXPECT_EQ(it->second, rit->second);
        ++it; ++rit;
    }

    EXPECT_EQ(it, map.end());
    EXPECT_EQ(rit, ref.end());
}
TEST(RBTreeMapStressTest, ManyFinds) {
    using Map = RBTreeMap<int, int>;
    Map map;

    const int N = 10000;
    const int SEARCH_OPS = 100000;

    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> keyDist(1, N);
    std::uniform_int_distribution<int> valDist(1, 1000);

    for (int i = 1; i <= N; ++i) map.insert({i, valDist(rng)});

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < SEARCH_OPS; ++i) {
        int key = keyDist(rng);
        auto it = map.find(key);
        (void)it;
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "[Stress] RBTree ManyFinds (" << SEARCH_OPS << " finds) done in "
              << elapsed.count() << " sec\n";

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < SEARCH_OPS; ++i) {
        int key = keyDist(rng);
        int v = map[key];
        (void)v;
    }
    end = std::chrono::steady_clock::now();
    elapsed = end - start;
    std::cout << "[Stress] RBTree ManyOperatorBrackets (" << SEARCH_OPS << " ops) done in "
              << elapsed.count() << " sec\n";
}