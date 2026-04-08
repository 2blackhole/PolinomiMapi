#ifndef OPENADDRESSINGHASHTABLEMAP_H
#define OPENADDRESSINGHASHTABLEMAP_H

 #include <cstddef>       
#include <utility>       
#include <iterator>  
#include "VectorBananov.h"
#include "VecIterator.h"

using std::pair;

template <class TKey, class TVal>
class OpenAddressingHashTableMap {
    using Pair = pair<TKey, TVal>;

    enum CellState { FREE, IN_USE, ERASED };

    struct HashCell {
        Pair data;
        CellState state;

        HashCell() : state(FREE) {}
        HashCell(const Pair& p, CellState s = IN_USE) : data(p), state(s) {}
    };

    VectorBananov<HashCell> table;
    size_t num_elements;
    size_t table_mask;
    const double LOAD_FACTOR_THRESHOLD = 0.75;

    size_t get_index(const TKey& key) const {
        return (std::hash<TKey>{}(key) * 22543) & table_mask;
    }

    void rehash() {
        size_t new_cap = (table_mask + 1) * 2;
        VectorBananov<HashCell> new_table(new_cap, HashCell());
        size_t new_mask = new_cap - 1;

        for (size_t i = 0; i < table.size(); ++i) {
            if (table[i].state == IN_USE) {
                size_t idx = (std::hash<TKey>{}(table[i].data.first) * 22543) & new_mask;
                while (new_table[idx].state == IN_USE) {
                    ++idx;
                    if (idx == new_cap) idx = 0;
                }
                new_table[idx].data = table[i].data;
                new_table[idx].state = IN_USE;
            }
        }

        table = std::move(new_table);
        table_mask = new_mask;
        num_elements = 0; 
        num_elements = 0;
        for (size_t i = 0; i < table.size(); ++i)
            if (table[i].state == IN_USE) ++num_elements;
    }

    class Iterator {
    public:
        using VecIter = typename VectorBananov<HashCell>::Iterator;
        VecIter it;
        VecIter end_it;

        Iterator(VecIter i, VecIter e) : it(i), end_it(e) {
            if (it != end_it && it->state != IN_USE) {
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
            while (it != end_it && it->state != IN_USE) {
                ++it;
            }
            return *this;
        }

        bool operator==(const Iterator& other) const { return it == other.it; }
        bool operator!=(const Iterator& other) const { return it != other.it; }
    };

public:
    OpenAddressingHashTableMap(size_t initial_size = 16)
        : num_elements(0) {
        size_t cap = 1;
        while (cap < initial_size) cap <<= 1;
        table_mask = cap - 1;
        table = VectorBananov<HashCell>(cap, HashCell());
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
        size_t idx = get_index(k);
        for (size_t i = 0; i <= table_mask; ++i) {
            HashCell& cell = table[idx];
            if (cell.state == FREE) return end();
            if (cell.state == IN_USE && cell.data.first == k)
                return Iterator(table.begin() + idx, table.end());
            ++idx;
            if (idx > table_mask) idx = 0;
        }
        return end();
    }

    bool empty() const { return num_elements == 0; }
    size_t size() const { return num_elements; }

    TVal& operator[](const TKey& k) {
        Iterator it = find(k);
        if (it != end()) return it->second;

        if (static_cast<double>(num_elements + 1) > LOAD_FACTOR_THRESHOLD * (table_mask + 1)) {
            rehash();
        }

        size_t idx = get_index(k);
        size_t first_free = table_mask + 1;
        for (size_t i = 0; i <= table_mask; ++i) {
            HashCell& cell = table[idx];
            if (cell.state == FREE || cell.state == ERASED) {
                if (first_free > table_mask) first_free = idx;
            }
            if (cell.state == IN_USE && cell.data.first == k) {
                                 return cell.data.second;
            }
            ++idx;
            if (idx > table_mask) idx = 0;
        }

                 HashCell& cell = table[first_free];
        cell.data = Pair(k, TVal());
        cell.state = IN_USE;
        ++num_elements;
        return cell.data.second;
    }

    Iterator erase(Iterator pos) {
        if (pos == end()) return end();
        size_t idx = pos.it - table.begin();
        table[idx].state = ERASED;
        --num_elements;
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
        Iterator it = find(p.first);
        if (it != end()) return {it, false};

        if (static_cast<double>(num_elements + 1) > LOAD_FACTOR_THRESHOLD * (table_mask + 1)) {
            rehash();
        }

        size_t idx = get_index(p.first);
        size_t first_free = table_mask + 1;
        for (size_t i = 0; i <= table_mask; ++i) {
            HashCell& cell = table[idx];
            if (cell.state == FREE || cell.state == ERASED) {
                if (first_free > table_mask) first_free = idx;
            }
            if (cell.state == IN_USE && cell.data.first == p.first) {
                                 return {Iterator(table.begin() + idx, table.end()), false};
            }
            ++idx;
            if (idx > table_mask) idx = 0;
        }

        HashCell& cell = table[first_free];
        cell.data = p;
        cell.state = IN_USE;
        ++num_elements;
        return {Iterator(table.begin() + first_free, table.end()), true};
    }

    pair<Iterator, bool> insert(Pair&& p) {
        Iterator it = find(p.first);
        if (it != end()) return {it, false};

        if (static_cast<double>(num_elements + 1) > LOAD_FACTOR_THRESHOLD * (table_mask + 1)) {
            rehash();
        }

        size_t idx = get_index(p.first);
        size_t first_free = table_mask + 1;
        for (size_t i = 0; i <= table_mask; ++i) {
            HashCell& cell = table[idx];
            if (cell.state == FREE || cell.state == ERASED) {
                if (first_free > table_mask) first_free = idx;
            }
            if (cell.state == IN_USE && cell.data.first == p.first) {
                return {Iterator(table.begin() + idx, table.end()), false};
            }
            ++idx;
            if (idx > table_mask) idx = 0;
        }

        HashCell& cell = table[first_free];
        cell.data = std::move(p);
        cell.state = IN_USE;
        ++num_elements;
        return {Iterator(table.begin() + first_free, table.end()), true};
    }

    Iterator begin() {
        return Iterator(table.begin(), table.end());
    }

    Iterator end() {
        return Iterator(table.end(), table.end());
    }
};

#endif  