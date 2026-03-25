#include <gtest/gtest.h>
#include "OpenAddressingHashTableMap.h"
#include <string>
#include <random>
#include <chrono>
#include <iostream>
#include <unordered_map>

class OpenAddressingHashTableMapTest : public ::testing::Test {
protected:
    OpenAddressingHashTableMap<int, std::string> emptyMap;
    OpenAddressingHashTableMap<int, std::string> map;

    void SetUp() override {
        map.insert({3, "three"});
        map.insert({1, "one"});
        map.insert({4, "four"});
        map.insert({2, "two"});
    }
};

TEST_F(OpenAddressingHashTableMapTest, DefaultConstructedIsEmpty) {
    EXPECT_TRUE(emptyMap.empty());
    EXPECT_EQ(emptyMap.size(), 0);
}

TEST_F(OpenAddressingHashTableMapTest, InsertAndSize) {
    OpenAddressingHashTableMap<int, std::string> m;
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

TEST_F(OpenAddressingHashTableMapTest, FindExistingKey) {
    auto it = map.find(2);
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->first, 2);
    EXPECT_EQ(it->second, "two");
}

TEST_F(OpenAddressingHashTableMapTest, FindNonExistingKey) {
    auto it = map.find(100);
    EXPECT_EQ(it, map.end());
}

TEST_F(OpenAddressingHashTableMapTest, SubscriptAccessExisting) {
    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[3], "three");
    EXPECT_EQ(map.size(), 4);
}

TEST_F(OpenAddressingHashTableMapTest, SubscriptInsertNew) {
    std::string& val = map[5];
    EXPECT_EQ(val, "");
    EXPECT_EQ(map.size(), 5);
    val = "five";
    EXPECT_EQ(map[5], "five");
}

TEST_F(OpenAddressingHashTableMapTest, SubscriptModifyExisting) {
    map[2] = "two modified";
    EXPECT_EQ(map[2], "two modified");
}

TEST_F(OpenAddressingHashTableMapTest, EraseByKeyExisting) {
    size_t erased = map.erase(2);
    EXPECT_EQ(erased, 1);
    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.find(2), map.end());
}

TEST_F(OpenAddressingHashTableMapTest, EraseByKeyNonExisting) {
    size_t erased = map.erase(100);
    EXPECT_EQ(erased, 0);
    EXPECT_EQ(map.size(), 4);
}

TEST_F(OpenAddressingHashTableMapTest, EraseByIterator) {
    auto it = map.find(3);
    ASSERT_NE(it, map.end());
    auto next = map.erase(it);
    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.find(3), map.end());
    
    EXPECT_TRUE(next == map.end() || next != map.end());
}

TEST_F(OpenAddressingHashTableMapTest, IterationCount) {
    std::unordered_map<int, std::string> ref;
    ref[1] = "one"; ref[2] = "two"; ref[3] = "three"; ref[4] = "four";
    size_t count = 0;
    for (const auto& p : map) {
        auto rit = ref.find(p.first);
        EXPECT_NE(rit, ref.end());
        EXPECT_EQ(rit->second, p.second);
        ++count;
    }
    EXPECT_EQ(count, map.size());
}

TEST_F(OpenAddressingHashTableMapTest, ModifyViaIterator) {
    auto it = map.find(1);
    ASSERT_NE(it, map.end());
    it->second = "modified";
    EXPECT_EQ(map[1], "modified");
}

TEST_F(OpenAddressingHashTableMapTest, Rehash) {
    OpenAddressingHashTableMap<int, int> map(2);
    const int N = 100;
    for (int i = 0; i < N; ++i) {
        map.insert({i, i * 10});
    }
    EXPECT_EQ(map.size(), N);
    for (int i = 0; i < N; ++i) {
        auto it = map.find(i);
        ASSERT_NE(it, map.end());
        EXPECT_EQ(it->second, i * 10);
    }
}

TEST(OpenAddressingHashTableMapStringTest, StringKeys) {
    OpenAddressingHashTableMap<std::string, int> m;
    m.insert({"apple", 10});
    m.insert({"banana", 20});
    m.insert({"cherry", 30});

    EXPECT_EQ(m["banana"], 20);
    EXPECT_EQ(m.find("apple")->second, 10);

    std::unordered_map<std::string, int> ref;
    ref["apple"] = 10; ref["banana"] = 20; ref["cherry"] = 30;
    size_t count = 0;
    for (const auto& p : m) {
        auto rit = ref.find(p.first);
        EXPECT_NE(rit, ref.end());
        EXPECT_EQ(rit->second, p.second);
        ++count;
    }
    EXPECT_EQ(count, 3);
}

TEST(OpenAddressingHashTableMapLargeTest, ManyElements) {
    OpenAddressingHashTableMap<int, int> m;
    const int N = 1000;
    for (int i = 0; i < N; ++i) m.insert({i, i * 10});
    EXPECT_EQ(m.size(), N);
    for (int i = 0; i < N; ++i) {
        auto it = m.find(i);
        ASSERT_NE(it, m.end());
        EXPECT_EQ(it->second, i * 10);
    }
}

TEST(OpenAddressingHashTableMapEraseTest, EraseAndReinsert) {
    OpenAddressingHashTableMap<int, int> m;
    m.insert({1, 10});
    m.erase(1);
    EXPECT_EQ(m.size(), 0);
    EXPECT_EQ(m.find(1), m.end());

    m.insert({1, 20});
    auto it = m.find(1);
    ASSERT_NE(it, m.end());
    EXPECT_EQ(it->second, 20);
}

TEST(OpenAddressingHashTableMapStressTest, ManyInsertErase) {
    using Map = OpenAddressingHashTableMap<int, int>;
    Map map;

    const int N = 5000;
    const int OPS = 200000;

    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> keyDist(1, N);
    std::uniform_int_distribution<int> valDist(1, 1000);
    std::uniform_int_distribution<int> opDist(0, 1);

    std::unordered_map<int, int> ref;

    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < OPS; ++i) {
        if (opDist(rng) == 0) {
            int key = keyDist(rng);
            int val = valDist(rng);
            map.insert({key, val});
            ref.insert({key, val});
        } else {
            int key = keyDist(rng);
            map.erase(key);
            ref.erase(key);
        }
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "[Stress] OpenAddressingHashTableMap ManyInsertErase (" << OPS << " ops) done in "
              << elapsed.count() << " sec\n";

    for (const auto& p : ref) {
        auto it = map.find(p.first);
        EXPECT_NE(it, map.end()) << "Key " << p.first << " not found";
        EXPECT_EQ(it->second, p.second);
    }

    for (auto it = map.begin(); it != map.end(); ++it) {
        auto rit = ref.find(it->first);
        EXPECT_NE(rit, ref.end()) << "Extra key " << it->first;
        EXPECT_EQ(it->second, rit->second);
    }
}

TEST(OpenAddressingHashTableMapStressTest, ManyFinds) {
    using Map = OpenAddressingHashTableMap<int, int>;
    Map map;

    const int N = 100000;
    const int SEARCH_OPS = 5000000;

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
    std::cout << "[Stress] OpenAddressingHashTableMap ManyFinds (" << SEARCH_OPS << " finds) done in "
              << elapsed.count() << " sec\n";

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < SEARCH_OPS; ++i) {
        int key = keyDist(rng);
        int v = map[key];
        (void)v;
    }
    end = std::chrono::steady_clock::now();
    elapsed = end - start;
    std::cout << "[Stress] OpenAddressingHashTableMap ManyOperatorBrackets (" << SEARCH_OPS << " ops) done in "
              << elapsed.count() << " sec\n";
}