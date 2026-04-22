#ifndef VECITERATOR_H
#define VECITERATOR_H

#include <iterator>
#include <cstddef>

template <typename Vector>
class VecIterator {
public:
    using value_type = typename Vector::value_type;
    using difference_type   = std::ptrdiff_t;
    using pointer_type = value_type*;
    using reference_type = value_type&;
    using iterator_category = std::random_access_iterator_tag;

    VecIterator(pointer_type ptr) : m_ptr(ptr) {}
    VecIterator() : m_ptr(nullptr) {}
    VecIterator(const VecIterator& it) : m_ptr(it.m_ptr) {}

    reference_type operator*() const { return *m_ptr; }
    pointer_type operator->() const { return m_ptr; }

    reference_type operator*() { return *m_ptr; }
    pointer_type operator->() { return m_ptr; }

    VecIterator& operator++() { ++m_ptr; return *this; }
    VecIterator operator++(int) { VecIterator it = *this; ++(*this); return it; }
    VecIterator& operator--() { --m_ptr; return *this; }
    VecIterator operator--(int) { VecIterator it = *this; --(*this); return it; }
    VecIterator& operator+=(difference_type n) { m_ptr += n; return *this; }
    VecIterator& operator-=(difference_type n) { m_ptr -= n; return *this; }

    friend VecIterator operator+(VecIterator it, difference_type n) { return it += n; }
    friend VecIterator operator+(difference_type n, VecIterator it) { return it += n; }
    friend VecIterator operator-(VecIterator it, difference_type n) { return it -= n; }
    friend difference_type operator-(const VecIterator& a, const VecIterator& b) { return a.m_ptr - b.m_ptr; }

    VecIterator& operator=(const VecIterator& other) {
        if (this != &other) m_ptr = other.m_ptr;
        return *this;
    }

    value_type operator[](difference_type n) const { return *(m_ptr + n); }

    bool operator==(const VecIterator& other) const { return m_ptr == other.m_ptr; }
    bool operator!=(const VecIterator& other) const { return m_ptr != other.m_ptr; }
    bool operator<(const VecIterator& other) const  { return m_ptr < other.m_ptr; }
    bool operator<=(const VecIterator& other) const { return m_ptr <= other.m_ptr; }
    bool operator>(const VecIterator& other) const  { return m_ptr > other.m_ptr; }
    bool operator>=(const VecIterator& other) const { return m_ptr >= other.m_ptr; }

private:
    pointer_type m_ptr = nullptr;
};

#endif //VECITERATOR_H