//
// Created by banani on 03/03/26.
//

#ifndef ORDEREDMAPVEC_H
#define ORDEREDMAPVEC_H

#include <cstddef>      
#include <utility>     
#include <iterator> 
#include "VectorBananov.h"
#include "VecIterator.h"
#include "Table.hpp"

using std::pair;

template <class TKey, class TVal>
class OrderedMapVec : public Table<TKey, TVal> {
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
        Iterator& operator+=(difference_type n) {
            iter += n; return *this;
        }
        bool operator!=(const Iterator& other) const { return iter != other.iter; }
        bool operator==(const Iterator & other) const {return iter == other.iter; }
    };

public:
    OrderedMapVec() = default;
    OrderedMapVec(const VectorBananov<pair<TKey, TVal>>& v) : data(v) {}
    OrderedMapVec(VectorBananov<pair<TKey, TVal>> v) : data(v) {}

    Iterator find(const TKey& k) {
        size_t l = 0;
        size_t r = data.size();
        while (l < r) {
            size_t mid = l + (r - l) / 2;
            if (data[mid].first == k) {
                return Iterator{data.begin() + mid};
            }
            if (data[mid].first < k) {
                l = mid + 1;
            }
            else {
                r = mid;
            }
        }
        return end();
    }

    TVal& operator[](const TKey& k) {
        auto [it, inserted] = insert({k, TVal()});
        return it->second;
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
            size_t l = 0;
            size_t r = data.size();
            while (l < r) {
                size_t mid = l + (r - l ) / 2;
                if (data[mid].first < p.first) {
                    l = mid + 1;
                }
                else {
                    r = mid;
                }
            }
            data.insert(data.begin() + l, p);
            return {Iterator{data.begin() + l}, true};
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