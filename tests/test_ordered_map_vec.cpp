#include <gtest/gtest.h>
#include <OrderedMapVec.h>
#include <string>
#include <random>
#include <chrono>
#include <iostream>
#include <map>
#include <vector>
class OrderedMapVecTest : public ::testing::Test {
protected:
    OrderedMapVec<int, std::string> emptyMap;
    OrderedMapVec<int, std::string> map;

    void SetUp() override {
        map.insert({3, "three"});
        map.insert({1, "one"});
        map.insert({4, "four"});
        map.insert({2, "two"});
    }
};


TEST_F(OrderedMapVecTest, DefaultConstructedIsEmpty) {
    EXPECT_TRUE(emptyMap.empty());
    EXPECT_EQ(emptyMap.size(), 0);
}

TEST_F(OrderedMapVecTest, InsertAndSize) {
    OrderedMapVec<int, std::string> m;
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

TEST_F(OrderedMapVecTest, FindExistingKey) {
    auto it = map.find(2);
    ASSERT_NE(it, map.end());
    EXPECT_EQ(it->first, 2);
    EXPECT_EQ(it->second, "two");
}

TEST_F(OrderedMapVecTest, FindNonExistingKey) {
    auto it = map.find(100);
    EXPECT_EQ(it, map.end());
}

TEST_F(OrderedMapVecTest, SubscriptAccessExisting) {
    EXPECT_EQ(map[1], "one");
    EXPECT_EQ(map[3], "three");
    EXPECT_EQ(map.size(), 4);
}

TEST_F(OrderedMapVecTest, SubscriptInsertNew) {
    std::string& val = map[5];
    EXPECT_EQ(val, "");
    EXPECT_EQ(map.size(), 5);
    val = "five";
    EXPECT_EQ(map[5], "five");
}

TEST_F(OrderedMapVecTest, SubscriptModifyExisting) {
    map[2] = "two modified";
    EXPECT_EQ(map[2], "two modified");
}

TEST_F(OrderedMapVecTest, EraseByKeyExisting) {
    size_t erased = map.erase(2);
    EXPECT_EQ(erased, 1);
    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.find(2), map.end());
}

TEST_F(OrderedMapVecTest, EraseByKeyNonExisting) {
    size_t erased = map.erase(100);
    EXPECT_EQ(erased, 0);
    EXPECT_EQ(map.size(), 4);
}

TEST_F(OrderedMapVecTest, EraseByIterator) {
    auto it = map.find(3);
    ASSERT_NE(it, map.end());
    auto next = map.erase(it);
    EXPECT_EQ(map.size(), 3);
    EXPECT_EQ(map.find(3), map.end());
    ASSERT_NE(next, map.end());
    EXPECT_EQ(next->first, 4);
}

TEST_F(OrderedMapVecTest, EraseByIteratorEnd) {
    auto it = map.find(4);
    auto next = map.erase(it);
    EXPECT_EQ(next, map.end());
    EXPECT_EQ(map.size(), 3);
}

TEST_F(OrderedMapVecTest, IterationOrder) {
    std::vector<int> expected = {1, 2, 3, 4};
    std::vector<int> actual;
    for (const auto& p : map) actual.push_back(p.first);
    EXPECT_EQ(actual, expected);
}

TEST_F(OrderedMapVecTest, InsertMaintainsOrder) {
    map.insert({0, "zero"});
    std::vector<int> expected = {0, 1, 2, 3, 4};
    std::vector<int> actual;
    for (const auto& p : map) actual.push_back(p.first);
    EXPECT_EQ(actual, expected);
}

TEST_F(OrderedMapVecTest, EraseMaintainsOrder) {
    map.erase(2);
    std::vector<int> expected = {1, 3, 4};
    std::vector<int> actual;
    for (const auto& p : map) actual.push_back(p.first);
    EXPECT_EQ(actual, expected);
}

TEST_F(OrderedMapVecTest, ModifyViaIterator) {
    auto it = map.find(1);
    ASSERT_NE(it, map.end());
    it->second = "modified";
    EXPECT_EQ(map[1], "modified");
}


TEST(OrderedMapVecStringTest, StringKeys) {
    OrderedMapVec<std::string, int> m;
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

TEST(OrderedMapVecLargeTest, ManyElements) {
    OrderedMapVec<int, int> m;
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

TEST(OrderedMapVecEraseTest, EraseAndReinsert) {
    OrderedMapVec<int, int> m;
    m.insert({1, 10});
    m.erase(1);
    EXPECT_EQ(m.size(), 0);
    EXPECT_EQ(m.find(1), m.end());

    m.insert({1, 20});
    auto it = m.find(1);
    ASSERT_NE(it, m.end());
    EXPECT_EQ(it->second, 20);
}

TEST(OrderedMapVecStressTest, ManyInsertErase) {
    using Map = OrderedMapVec<int, int>;
    Map map;

    const int N = 200000;
    const int OPS = 100000;

    std::mt19937 rng(12345);
    std::uniform_int_distribution<int> keyDist(1, N);
    std::uniform_int_distribution<int> valDist(1, 1000);
    std::uniform_int_distribution<int> opDist(0, 1);

    std::map<int, int> ref;

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
        if (i % (OPS / 10) == 0 && i > 0) {
            EXPECT_EQ(map.size(), ref.size());
        }
    }

    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "[Stress] ManyInsertErase (" << OPS << " ops) done in "
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

TEST(OrderedMapVecStressTest, ManyFinds) {
    using Map = OrderedMapVec<int, int>;
    Map map;

    const int N = 500'000;
    const int SEARCH_OPS = 5'000'000;
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
    std::cout << "[Stress] ManyFinds (" << SEARCH_OPS << " finds) done in "
              << elapsed.count() << " sec\n";

    start = std::chrono::steady_clock::now();
    for (int i = 0; i < SEARCH_OPS; ++i) {
        int key = keyDist(rng);
        int v = map[key];
        (void)v;
    }
    end = std::chrono::steady_clock::now();
    elapsed = end - start;
    std::cout << "[Stress] ManyOperatorBrackets (" << SEARCH_OPS << " ops) done in "
              << elapsed.count() << " sec\n";
}