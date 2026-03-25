#ifndef VECTORBANANOV_H
#define VECTORBANANOV_H

#include <iostream>
#include <algorithm>
#include <stdexcept>
#include "VecIterator.h"

template <typename T>
class VectorBananov {
public:
    using value_type = T;
    using pointer_type = T*;
    using reference_type = T&;
    using const_reference_type = const T&;
    using Iterator = VecIterator<VectorBananov<T>>;
    using ConstIterator = VecIterator<const VectorBananov<T>>;

private:
    pointer_type m_data = nullptr;
    size_t m_size = 0;
    size_t m_capacity = 0;

public:
    VectorBananov() = default;

    void Realloc(size_t new_capacity) {
        if (new_capacity == m_capacity) return;
        if (new_capacity == 0) {
            delete[] m_data;
            m_data = nullptr;
            m_capacity = 0;
            m_size = 0;
            return;
        }
        pointer_type new_chunk = new value_type[new_capacity];
        size_t to_copy = std::min(m_size, new_capacity);
        for (size_t i = 0; i < to_copy; ++i) new_chunk[i] = m_data[i];
        delete[] m_data;
        m_data = new_chunk;
        m_capacity = new_capacity;
        if (m_size > m_capacity) m_size = m_capacity;
    }

    VectorBananov(std::initializer_list<T> init_list) {
        size_t init_sz = init_list.size();
        size_t cap = std::max<size_t>(1, init_sz * 2 + 2);
        Realloc(cap);
        m_size = init_sz;
        size_t i = 0;
        for (const auto &v : init_list) m_data[i++] = v;
    }

    VectorBananov(size_t size, value_type init_value) {
        size_t cap = std::max<size_t>(1, size * 2 + 2);
        Realloc(cap);
        m_size = size;
        for (size_t i = 0; i < m_size; ++i) m_data[i] = init_value;
    }

    VectorBananov(pointer_type start, pointer_type end) {
        if (start > end) throw std::invalid_argument("The start pointer must be before the end pointer");
        size_t cnt = static_cast<size_t>(end - start);
        size_t cap = std::max<size_t>(1, cnt * 2);
        Realloc(cap);
        m_size = cnt;
        for (size_t i = 0; i < cnt; ++i) m_data[i] = start[i];
    }

    VectorBananov(Iterator start, Iterator end) {
        if (start > end) throw std::invalid_argument("The start iterator must be before the end iterator");
        size_t cnt = static_cast<size_t>(end - start);
        size_t cap = std::max<size_t>(1, cnt * 2);
        Realloc(cap);
        m_size = cnt;
        for (size_t i = 0; i < cnt; ++i) m_data[i] = *start++;
    }

    VectorBananov(const VectorBananov& other) {
        m_size = other.m_size;
        m_capacity = other.m_capacity;
        if (m_capacity > 0) {
            m_data = new value_type[m_capacity];
            for (size_t i = 0; i < m_size; ++i) m_data[i] = other.m_data[i];
        } else {
            m_data = nullptr;
        }
    }

    VectorBananov(VectorBananov&& other) noexcept
        : m_data(other.m_data), m_size(other.m_size), m_capacity(other.m_capacity) {
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
    }

    VectorBananov& operator=(const VectorBananov& other) {
        if (this == &other) return *this;
        VectorBananov tmp(other);
        swap(tmp);
        return *this;
    }

    VectorBananov& operator=(VectorBananov&& other) noexcept {
        if (this == &other) return *this;
        delete[] m_data;
        m_data = other.m_data;
        m_size = other.m_size;
        m_capacity = other.m_capacity;
        other.m_data = nullptr;
        other.m_size = 0;
        other.m_capacity = 0;
        return *this;
    }

    ~VectorBananov() {
        delete[] m_data;
        m_data = nullptr;
        m_size = 0;
        m_capacity = 0;
    }

    Iterator begin() { return Iterator(m_data); }
    Iterator end() { return Iterator(m_data + m_size); }
    ConstIterator begin() const { return ConstIterator(m_data); }
    ConstIterator end() const { return ConstIterator(m_data + m_size); }

    reference_type operator[](size_t index) { return m_data[index]; }
    const_reference_type operator[](size_t index) const { return m_data[index]; }

    reference_type at(size_t index) {
        if (index >= m_size) throw std::out_of_range("Index out of range");
        return m_data[index];
    }
    const_reference_type at(size_t index) const {
        if (index >= m_size) throw std::out_of_range("Index out of range");
        return m_data[index];
    }

    reference_type back() {
        if (m_size == 0) throw std::out_of_range("Vector is empty");
        return m_data[m_size - 1];
    }

    const_reference_type back() const {
        if (m_size == 0) throw std::out_of_range("Vector is empty");
        return m_data[m_size - 1];
    }
    
    const pointer_type data() const { return m_data; }
    pointer_type data() { return m_data; }

    bool empty() const { return m_size == 0; }
    size_t size() const { return m_size; }
    size_t capacity() const { return m_capacity; }

    size_t insert(size_t pos, const value_type& ch) {
        if (pos > m_size) throw std::out_of_range("Index out of range");
        if (m_size + 1 > m_capacity) {
            size_t newCap = std::max<size_t>(1, m_capacity == 0 ? 2 : m_capacity * 2);
            Realloc(newCap);
        }
        for (size_t i = m_size; i > pos; --i) m_data[i] = m_data[i - 1];
        m_data[pos] = ch;
        ++m_size;
        return pos;
    }

    size_t insert(size_t pos, const VectorBananov &other) {
        if (pos > m_size) throw std::out_of_range("Index out of range");
        size_t required = m_size + other.m_size;
        if (required > m_capacity) {
            size_t newCap = std::max(required, std::max<size_t>(1, m_capacity * 2));
            Realloc(newCap);
        }
        for (size_t i = m_size; i > pos; --i) m_data[i + other.m_size - 1] = m_data[i - 1];
        for (size_t i = 0; i < other.m_size; ++i) m_data[pos + i] = other.m_data[i];
        m_size += other.m_size;
        return pos;
    }

    Iterator insert(Iterator it, const VectorBananov& other) {
        size_t pos = it - begin();
        insert(pos, other);
        return Iterator(m_data + pos);
    }

    Iterator insert(Iterator it, const value_type& ch) {
        size_t pos = it - begin();
        insert(pos, ch);
        return Iterator(m_data + pos);
    }

    size_t erase(size_t pos) {
        if (m_size == 0) throw std::invalid_argument("Vector must not be empty");
        if (pos >= m_size) throw std::out_of_range("Pos out of range");
        for (size_t i = pos; i + 1 < m_size; ++i) m_data[i] = m_data[i + 1];
        --m_size;
        return pos;
    }

    size_t erase(size_t pos, size_t count) {
        if (m_size == 0) throw std::invalid_argument("Vector is empty");
        if (pos >= m_size) throw std::out_of_range("Position out of range");
        if (count == 0 || pos + count > m_size) count = m_size - pos;
        for (size_t i = pos; i + count < m_size; ++i) m_data[i] = m_data[i + count];
        m_size -= count;
        return pos;
    }

    Iterator erase(Iterator st, Iterator e) {
        size_t pos = st - begin();
        size_t cnt = e - st;
        erase(pos, cnt);
        return Iterator(m_data + pos);
    }

    Iterator erase(Iterator iter) {
        size_t pos = iter - begin();
        erase(pos);
        return Iterator(m_data + pos);
    }

    void push_back(value_type&& ch) {
        if (m_size + 1 > m_capacity) {
            size_t newCap = std::max<size_t>(1, m_capacity == 0 ? 2 : m_capacity * 2);
            Realloc(newCap);
        }
        m_data[m_size++] = std::move(ch);
    }

    void push_back(const const_reference_type ch) {
        if (m_size + 1 > m_capacity) {
            size_t newCap = std::max<size_t>(1, m_capacity == 0 ? 2 : m_capacity * 2);
            Realloc(newCap);
        }
        m_data[m_size++] = ch;
    }

    void pop_back() {
        if (m_size == 0) return;
        --m_size;
    }

    void swap(VectorBananov& other) noexcept {
        std::swap(other.m_data, m_data);
        std::swap(other.m_size, m_size);
        std::swap(other.m_capacity, m_capacity);
    }

    void clear() { m_size = 0; }

    void resize(size_t count) {
        if (count > m_capacity) Realloc(std::max(count, m_capacity * 2));
        m_size = count;
    }

    friend std::ostream& operator<<(std::ostream& out, const VectorBananov& vec) {
        if (vec.m_size == 0) {
            out << "[ ]";
            return out;
        }
        out << "[ ";
        for (size_t i = 0; i < vec.size() - 1; i++) out << vec[i] << ", ";
        out << vec[vec.size() - 1] << " ]";
        return out;
    }
};

template <typename T>
bool operator==(const VectorBananov<T>& a, const VectorBananov<T>& b) {
    if (a.size() != b.size()) return false;
    for (size_t i = 0; i < a.size(); ++i) if (!(a[i] == b[i])) return false;
    return true;
}
template <typename T> bool operator!=(const VectorBananov<T>& a, const VectorBananov<T>& b) { return !(a == b); }
template <typename T>
bool operator<(const VectorBananov<T>& a, const VectorBananov<T>& b) {
    return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}
template <typename T> bool operator<=(const VectorBananov<T>& a, const VectorBananov<T>& b) { return !(b < a); }
template <typename T> bool operator>(const VectorBananov<T>& a, const VectorBananov<T>& b) { return b < a; }
template <typename T> bool operator>=(const VectorBananov<T>& a, const VectorBananov<T>& b) { return !(a < b); }

#endif
