//
// Created by banani on 03/03/26.
//

#ifndef ORDEREDMAPVEC_H
#define ORDEREDMAPVEC_H

#include <bits/stdc++.h>
#include "VectorBananov.h"
#include "VecIterator.h"
using std::pair;

template <class TKey, class TVal>
class OrderedMapVec {
    using Pair = pair<TKey, TVal>;
    VectorBananov<Pair> data;

    class Iterator {
    public:
        VecIterator<VectorBananov<Pair>> iter;
        explicit Iterator(VecIterator<VectorBananov<Pair>> i) : iter(i) {}

        using iterator_category = std::forward_iterator_tag;
        using value_type = Pair;
        using difference_type = std::ptrdiff_t;
        using posize_ter = Pair*;
        using reference = Pair&;

        reference operator*() const { return *iter; }
        posize_ter operator->() const { return &(*iter); }

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


    bool empty() {
        return data.empty();
    }

    size_t size() {
        return data.size();
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

};

#endif //ORDEREDMAPVEC_H
