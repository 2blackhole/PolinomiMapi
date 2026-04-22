#ifndef FORWARDLISTITERATOR_HPP
#define FORWARDLISTITERATOR_HPP

#include <iterator>
#include <type_traits>
#include <utility>
#include <cstddef>

template <class T>
struct FLNode {
    T data;
    FLNode* next = nullptr;
    FLNode(const T& v) : data(v) {}
    FLNode(T&& v) noexcept(std::is_nothrow_move_constructible<T>::value) : data(std::move(v)) {}
};

template<class T, bool Const>
class ForwardListIterator {
    using node_t = FLNode<T>;
    node_t* ptr = nullptr;

    using val_t = std::conditional_t<Const, const T, T>;
    using ref_t = std::add_lvalue_reference_t<val_t>;
    using ptr_t = std::add_pointer_t<val_t>;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using reference = ref_t;
    using pointer = ptr_t;

    ForwardListIterator() = default;
    explicit ForwardListIterator(node_t* n) : ptr(n) {}

    template<class U, bool C2>
    friend class ForwardListIterator;

    template<bool C = Const, class = std::enable_if_t<C>>
    ForwardListIterator(const ForwardListIterator<T, false>& it) : ptr(it.ptr) {}

    reference operator*() const { return ptr->data; }
    pointer operator->() const { return std::addressof(ptr->data); }

    ForwardListIterator& operator++() { ptr = ptr->next; return *this; }
    ForwardListIterator operator++(int) { ForwardListIterator tmp(*this); ptr = ptr->next; return tmp; }

    friend bool operator==(const ForwardListIterator& a, const ForwardListIterator& b) { return a.ptr == b.ptr; }
    friend bool operator!=(const ForwardListIterator& a, const ForwardListIterator& b) { return a.ptr != b.ptr; }

    template<class> friend class ForwardList;
};


#endif
