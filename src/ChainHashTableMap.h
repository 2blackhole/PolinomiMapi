//
// Created by Amour on 2026-03-18.
//

#ifndef CHAIN_HASH_TABLE_MAP_H
#define CHAIN_HASH_TABLE_MAP_H

#include <cstddef>       
#include <functional>   
#include <utility>      
#include <iterator>  
#include "VectorBananov.h"
#include "VecIterator.h"
#include "Table.hpp"

using std::pair;

template <class TKey, class TVal>
class ChainHashTableMap : public Table<TKey, TVal> {
    using Pair = pair<TKey, TVal>;
    using Bucket = VectorBananov<Pair>;
    using Buckets = VectorBananov<Bucket>;

    Buckets buckets;
    size_t sz;

    size_t hash(const TKey& k) const {
        return std::hash<TKey>{}(k) % buckets.size();
    }

    void rehash(size_t new_bucket_count) {
        Buckets old_buckets = std::move(buckets);
        buckets = Buckets(new_bucket_count, Bucket());
        for (size_t i = 0; i < buckets.size(); ++i) {
            buckets[i] = Bucket();
        }
        sz = 0;
        for (size_t i = 0; i < old_buckets.size(); ++i) {
            for (size_t j = 0; j < old_buckets[i].size(); ++j) {
                insert(old_buckets[i][j]);
            }
        }
    }

public:
    class Iterator {
    public:
        using OI = VecIterator<Buckets>;
        using II = VecIterator<Bucket>;

        OI outer;
        OI outer_end;
        II inner;

        Iterator(OI o, OI oe) : outer(o), outer_end(oe) {
            if (outer != outer_end) {
                inner = outer->begin();
                if (outer->empty()) {
                    ++(*this);
                }
            }
        }

        Iterator(OI o, OI oe, II i) : outer(o), outer_end(oe), inner(i) {}

        using iterator_category = std::forward_iterator_tag;
        using value_type = Pair;
        using difference_type = std::ptrdiff_t;
        using pointer = Pair*;
        using reference = Pair&;

        reference operator*() const { return *inner; }
        pointer operator->() const { return &(*inner); }

        Iterator& operator++() {
            ++inner;
            while (inner == outer->end()) {
                ++outer;
                if (outer == outer_end) break;
                inner = outer->begin();
            }
            return *this;
        }

        bool operator==(const Iterator& other) const {
            if (outer == outer_end && other.outer == other.outer_end) return true;
            if (outer == outer_end || other.outer == other.outer_end) return false;
            return outer == other.outer && inner == other.inner;
        }

        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
    };

    ChainHashTableMap(size_t initial_buckets = 16) : sz(0) {
        buckets = Buckets(initial_buckets, Bucket());
        for (size_t i = 0; i < buckets.size(); ++i) {
            buckets[i] = Bucket();
        }
    }

    ChainHashTableMap(const VectorBananov<pair<TKey, TVal>>& v) : ChainHashTableMap() {
        for (const auto& p : v) {
            insert(p);
        }
    }

    ChainHashTableMap(VectorBananov<pair<TKey, TVal>>&& v) : ChainHashTableMap() {
        for (auto& p : v) {
            insert(std::move(p));
        }
    }

    Iterator find(const TKey& k) {
        size_t idx = hash(k);
        Bucket& chain = buckets[idx];
        for (auto it = chain.begin(); it != chain.end(); ++it) {
            if (it->first == k) {
                return Iterator(buckets.begin() + idx, buckets.end(), it);
            }
        }
        return end();
    }

    TVal& operator[](const TKey& k) {
        Iterator it = find(k);
        if (it != end()) {
            return it->second;
        }
        auto res = insert({k, TVal()});
        return res.first->second;
    }

    Iterator erase(Iterator pos) {
        if (pos == end()) return end();

        size_t idx = pos.outer - buckets.begin();
        Bucket& chain = buckets[idx];
        auto next_inner = chain.erase(pos.inner);
        --sz;

        if (!chain.empty() && next_inner != chain.end()) {
            return Iterator(pos.outer, buckets.end(), next_inner);
        } else {
            Iterator next = pos;
            ++next;
            return next;
        }
    }

    size_t erase(const TKey& k) {
        Iterator it = find(k);
        if (it != end()) {
            erase(it);
            return 1;
        }
        return 0;
    }

    pair<Iterator, bool> insert(const Pair& p) {
        size_t idx = hash(p.first);
        Bucket& chain = buckets[idx];
        for (auto it = chain.begin(); it != chain.end(); ++it) {
            if (it->first == p.first) {
                return {Iterator(buckets.begin() + idx, buckets.end(), it), false};
            }
        }
        chain.push_back(p);
        ++sz;

        if (sz > buckets.size()) {
            rehash(buckets.size() * 2);
            return {find(p.first), true};
        }
        return {Iterator(buckets.begin() + idx, buckets.end(), chain.end() - 1), true};
    }

    pair<Iterator, bool> insert(Pair&& p) {
        size_t idx = hash(p.first);
        Bucket& chain = buckets[idx];
        for (auto it = chain.begin(); it != chain.end(); ++it) {
            if (it->first == p.first) {
                return {Iterator(buckets.begin() + idx, buckets.end(), it), false};
            }
        }
        chain.push_back(std::move(p));
        ++sz;

        if (sz > buckets.size()) {
            rehash(buckets.size() * 2);
            return {find(p.first), true};
        }
        return {Iterator(buckets.begin() + idx, buckets.end(), chain.end() - 1), true};
    }

    Iterator begin() {
        if (empty()) return end();
        for (auto outer = buckets.begin(); outer != buckets.end(); ++outer) {
            if (!outer->empty()) {
                return Iterator(outer, buckets.end(), outer->begin());
            }
        }
        return end();
    }

    Iterator end() {
        return Iterator(buckets.end(), buckets.end());
    }

    void add(const TKey& key, const TVal& value) override {
        this->insert({key, value});
    }

    bool remove(const TKey& key) override {
        return this->erase(key) > 0;
    }

    TVal* get(const TKey& key) override {
        Iterator it = this->find(key);
        return (it != end()) ? &(it->second) : nullptr;
    }

    size_t size() override {
        return sz;
    }

    bool empty() override {
        return sz == 0;
    }

    void clear() override {
        buckets = Buckets(buckets.size(), Bucket());
        sz = 0;
    }

    std::vector<TKey> keys() override {
        std::vector<TKey> res;
        for (auto it = begin(); it != end(); ++it)
            res.push_back(it->first);
        return res;
    }

    std::vector<std::pair<TKey, TVal>> items() override {
        std::vector<std::pair<TKey, TVal>> res;
        for (auto it = begin(); it != end(); ++it)
            res.emplace_back(it->first, it->second);
        return res;
    }

    bool contains(const TKey& key) override {
        return find(key) != end();
    }
};

#endif