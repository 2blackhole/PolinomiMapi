//
// Created by Amour on 26/02/26.
//
#ifndef UNORDERED_MAP_VEC_H
#define UNORDERED_MAP_VEC_H

#include <cstddef>
#include <utility>
#include <iterator>
#include "VectorBananov.h"
#include "VecIterator.h"
#include "Table.hpp"

using std::pair;

template <class TKey, class TVal>
class UnorderedMapVec : public Table<TKey, TVal> {
    using Pair = pair<TKey, TVal>;
    VectorBananov<Pair> data;

    class Iterator {
    public:
        VecIterator<VectorBananov<Pair>> iter;
        explicit Iterator(VecIterator<VectorBananov<Pair>> i) : iter(i) {}

        using iterator_category = std::forward_iterator_tag;
        using value_type = Pair;
        using difference_type = std::ptrdiff_t;
        using pointer = Pair*;
        using reference = Pair&;

        reference operator*() const { return *iter; }
        pointer operator->() const { return &(*iter); }

        Iterator& operator++() { ++iter; return *this; }
        bool operator!=(const Iterator& other) const { return iter != other.iter; }
        bool operator==(const Iterator & other) const {return iter == other.iter; }
    };

public:
    UnorderedMapVec() = default;
    UnorderedMapVec(const VectorBananov<pair<TKey, TVal>>& v) : data(v) {}
    UnorderedMapVec(VectorBananov<pair<TKey, TVal>> v) : data(v) {}

    Iterator find(const TKey& k) {
        for (Iterator it = this->begin(); it != this->end(); ++it) {
            if (it->first == k) {
                return it;
            }
        }
        return this->end();
    }

    TVal& operator[](const TKey& k) {
        Iterator it = find(k);
        if (it != end()) {
            return it->second;
        }
        data.push_back({k, TVal()});
        return data.back().second;
    }

    Iterator erase(Iterator pos) {
        return Iterator(data.erase(pos.iter));
    }

    size_t erase(const TKey& k) {
        Iterator it = find(k);
        if (it != end()) {
            erase(it);
            return 1;
        } else {
            return 0;
        }
    }

    pair<Iterator, bool> insert(const Pair& p) {
        if (find(p.first) == end()) {
            data.push_back(p);
            return {Iterator{data.end() - 1}, true};
        }
        return {find(p.first), false};
    }

    Iterator begin() {
        return Iterator{data.begin()};
    }

    Iterator end() {
        return Iterator{data.end()};
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
        return data.size();
    }

    bool empty() override {
        return data.empty();
    }

    void clear() override {
        data.clear();
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