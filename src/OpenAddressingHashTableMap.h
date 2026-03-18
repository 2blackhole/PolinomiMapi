//
// Created by Egorb on 2026-03-19.
//

#ifndef OPENADDRESSINGHASHTABLEMAP_H
#define OPENADDRESSINGHASHTABLEMAP_H

#ifndef OPEN_ADDRESSING_HASH_TABLE_MAP_H
#define OPEN_ADDRESSING_HASH_TABLE_MAP_H

#include <bits/stdc++.h>
#include "VectorBananov.h"
#include "VecIterator.h"

using std::pair;

template <class TKey, class TVal>
class OpenAddressingHashTableMap {
    using Pair = pair<TKey, TVal>;

    enum CellState { EMPTY, OCCUPIED, DELETED };

    struct HashCell {
        Pair data;
        CellState state;

        HashCell() : state(EMPTY) {}
        HashCell(const Pair& p, CellState s = OCCUPIED) : data(p), state(s) {}
    };

    VectorBananov<HashCell> table;
    size_t num_elements;
    size_t num_deleted;
    const double LOAD_FACTOR_THRESHOLD = 0.75;

    size_t hash1(const TKey& k) const {
        return std::hash<TKey>{}(k);
    }

    size_t hash2(const TKey& k) const {
        size_t h = std::hash<TKey>{}(k);
        return (h % (table.size() - 1)) + 1;
    }

    size_t probe(const TKey& k, size_t i) const {
        return (hash1(k) + i * hash2(k)) % table.size();
    }

    void rehash(size_t new_size) {
        VectorBananov<HashCell> old_table = std::move(table);
        table = VectorBananov<HashCell>(new_size);
        for (size_t j = 0; j < table.size(); ++j) {
            table[j].state = EMPTY;
        }
        num_elements = 0;
        num_deleted = 0;

        for (size_t i = 0; i < old_table.size(); ++i) {
            if (old_table[i].state == OCCUPIED) {
                insert(old_table[i].data);
            }
        }
    }

    class Iterator {
    public:
        using VecIter = typename VectorBananov<HashCell>::iterator;
        VecIter it;
        VecIter end_it;

        Iterator(VecIter i, VecIter e) : it(i), end_it(e) {
            if (it != end_it && it->state != OCCUPIED) {
                ++(*this);
            }
        }

        using iterator_category = std::forward_iterator_tag;
        using value_type = Pair;
        using difference_type = std::ptrdiff_t;
        using pointer = Pair*;
        using reference = Pair&;

        reference operator*() const { return it->data; }
        pointer operator->() const { return &(it->data); }

        Iterator& operator++() {
            ++it;
            while (it != end_it && it->state != OCCUPIED) {
                ++it;
            }
            return *this;
        }

        bool operator==(const Iterator& other) const { return it == other.it; }
        bool operator!=(const Iterator& other) const { return it != other.it; }
    };

public:
    OpenAddressingHashTableMap(size_t initial_size = 16)
        : table(initial_size), num_elements(0), num_deleted(0) {
        for (size_t i = 0; i < table.size(); ++i) {
            table[i].state = EMPTY;
        }
    }

    OpenAddressingHashTableMap(const VectorBananov<pair<TKey, TVal>>& v)
        : OpenAddressingHashTableMap(v.size() * 2) {
        for (const auto& p : v) {
            insert(p);
        }
    }

    OpenAddressingHashTableMap(VectorBananov<pair<TKey, TVal>>&& v)
        : OpenAddressingHashTableMap(v.size() * 2) {
        for (auto& p : v) {
            insert(std::move(p));
        }
    }

    Iterator find(const TKey& k) {
        for (size_t i = 0; i < table.size(); ++i) {
            size_t idx = probe(k, i);
            HashCell& cell = table[idx];

            if (cell.state == EMPTY) {
                return end();
            }
            if (cell.state == OCCUPIED && cell.data.first == k) {
                return Iterator(table.begin() + idx, table.end());
            }
        }
        return end();
    }

    bool empty() const {
        return num_elements == 0;
    }

    size_t size() const {
        return num_elements;
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

        size_t idx = pos.it - table.begin();
        table[idx].state = DELETED;
        --num_elements;
        ++num_deleted;

        Iterator next = pos;
        ++next;
        return next;
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
        if (static_cast<double>(num_elements + num_deleted) > LOAD_FACTOR_THRESHOLD * table.size()) {
            rehash(table.size() * 2);
        }

        for (size_t i = 0; i < table.size(); ++i) {
            size_t idx = probe(p.first, i);
            HashCell& cell = table[idx];

            if (cell.state == OCCUPIED && cell.data.first == p.first) {
                return {Iterator(table.begin() + idx, table.end()), false};
            }

            if (cell.state != OCCUPIED) {
                cell.data = p;
                cell.state = OCCUPIED;
                ++num_elements;
                return {Iterator(table.begin() + idx, table.end()), true};
            }
        }

        rehash(table.size() * 2);
        return insert(p);
    }

    pair<Iterator, bool> insert(Pair&& p) {
        if (static_cast<double>(num_elements + num_deleted) > LOAD_FACTOR_THRESHOLD * table.size()) {
            rehash(table.size() * 2);
        }

        for (size_t i = 0; i < table.size(); ++i) {
            size_t idx = probe(p.first, i);
            HashCell& cell = table[idx];

            if (cell.state == OCCUPIED && cell.data.first == p.first) {
                return {Iterator(table.begin() + idx, table.end()), false};
            }

            if (cell.state != OCCUPIED) {
                cell.data = std::move(p);
                cell.state = OCCUPIED;
                ++num_elements;
                return {Iterator(table.begin() + idx, table.end()), true};
            }
        }

        rehash(table.size() * 2);
        return insert(p);
    }

    Iterator begin() {
        return Iterator(table.begin(), table.end());
    }

    Iterator end() {
        return Iterator(table.end(), table.end());
    }
};

#endif //OPENADDRESSINGHASHTABLEMAP_H
