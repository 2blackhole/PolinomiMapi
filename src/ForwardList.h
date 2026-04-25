#ifndef FORWARDLIST_HPP
#define FORWARDLIST_HPP

#include <cstddef>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <unordered_set>
#include <initializer_list>
#include <iterator>
#include <functional>
#include <limits>
#include <string>
#include <vector>

#include "ForwardListIterator.h"

template<class T>
class ForwardList {
    using node_t = FLNode<T>;
    node_t* head = nullptr;
    std::size_t sz = 0;

public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using iterator = ForwardListIterator<T, false>;
    using const_iterator = ForwardListIterator<T, true>;

    ForwardList() = default;

    ForwardList(const ForwardList& other) {
        for (const auto& x : other) {
            push_front(x);
        }
        reverse();
    }

    ForwardList(ForwardList&& other) noexcept : head(other.head), sz(other.sz) {
        other.head = nullptr;
        other.sz = 0;
    }

    ForwardList(std::initializer_list<T> il) {
        insert_range_after(before_begin_internal_(), il);
    }

    ~ForwardList() {
        clear();
    }

    ForwardList& operator=(const ForwardList& other) {
        if (this == &other) return *this;
        ForwardList tmp(other);
        swopa(tmp);
        return *this;
    }

    ForwardList& operator=(ForwardList&& other) noexcept {
        if (this == &other) return *this;
        clear();
        head = other.head;
        sz = other.sz;
        other.head = nullptr;
        other.sz = 0;
        return *this;
    }

    ForwardList& operator=(std::initializer_list<T> il) {
        clear();
        insert_range_after(before_begin_internal_(), il);
        return *this;
    }

    iterator begin() noexcept { return iterator(head); }
    const_iterator begin() const noexcept { return const_iterator(head); }
    const_iterator cbegin() const noexcept { return const_iterator(head); }

    iterator end() noexcept { return iterator(nullptr); }
    const_iterator end() const noexcept { return const_iterator(nullptr); }
    const_iterator cend() const noexcept { return const_iterator(nullptr); }

    reference front() {
        if (!head) throw std::out_of_range("List IS .... emPty.........");
        return head->data;
    }

    const_reference front() const {
        if (!head) throw std::out_of_range("List IS .... emPty.........");
        return head->data;
    }

    bool empty() const noexcept { return sz == 0; }
    size_type size() const noexcept { return sz; }
    size_type max_size() const noexcept { return std::numeric_limits<size_type>::max() / sizeof(node_t); }

    void clear() noexcept {
        node_t* cur = head;
        while (cur) {
            node_t* nx = cur->next;
            delete cur;
            cur = nx;
        }
        head = nullptr;
        sz = 0;
    }

    void push_front(const T& v) {
        node_t* n = new node_t(v);
        n->next = head;
        head = n;
        ++sz;
    }

    void push_front(T&& v) {
        node_t* n = new node_t(std::move(v));
        n->next = head;
        head = n;
        ++sz;
    }

    reference emplace_front(const std::vector<T>& values) {
        if (values.empty()) throw std::invalid_argument("emplace_front(vector) empty");
        for (auto it = values.rbegin(); it != values.rend(); ++it) {
            node_t* n = new node_t(*it);
            n->next = head;
            head = n;
            ++sz;
        }
        return head->data;
    }

    void pop_front() {
        if (!head) return;
        node_t* n = head;
        head = head->next;
        delete n;
        --sz;
    }

    iterator insert_after(const_iterator pos, const T& value) {
        check_iter_single_(pos);
        node_t* p = pos.ptr;
        node_t* n = new node_t(value);
        if (!p) {
            n->next = head;
            head = n;
        } else {
            n->next = p->next;
            p->next = n;
        }
        ++sz;
        return iterator(n);
    }

    iterator insert_after(const_iterator pos, T&& value) {
        check_iter_single_(pos);
        node_t* p = pos.ptr;
        node_t* n = new node_t(std::move(value));
        if (!p) {
            n->next = head;
            head = n;
        } else {
            n->next = p->next;
            p->next = n;
        }
        ++sz;
        return iterator(n);
    }

    iterator insert_after(const_iterator pos, size_type count, const T& value) {
        check_iter_single_(pos);
        node_t* last = pos.ptr;
        for (size_type i = 0; i < count; ++i) {
            last = insert_after(const_iterator(last), value).ptr;
        }
        return iterator(last);
    }

    template<class InputIt,
        class = typename std::enable_if<!std::is_integral<InputIt>::value>::type>
        iterator insert_after(const_iterator pos, InputIt first, InputIt last) {
        check_iter_single_(pos);
        node_t* cur = pos.ptr;
        for (; first != last; ++first) {
            cur = insert_after(const_iterator(cur), *first).ptr;
        }
        return iterator(cur);
    }

    iterator insert_after(const_iterator pos, std::initializer_list<T> il) {
        return insert_after(pos, il.begin(), il.end());
    }

    template<class R>
    iterator insert_range_after(const_iterator pos, R&& r) {
        using std::begin;
        using std::end;
        return insert_after(pos, begin(r), end(r));
    }

    template<class... Args>
    iterator emplace_after(const_iterator pos, Args&&... args) {
        check_iter_single_(pos);
        node_t* p = pos.ptr;
        node_t* n = new node_t(T(std::forward<Args>(args)...));
        if (!p) {
            n->next = head;
            head = n;
        } else {
            n->next = p->next;
            p->next = n;
        }
        ++sz;
        return iterator(n);
    }

    iterator erase_after(const_iterator pos) {
        check_iter_single_(pos);
        node_t* p = pos.ptr;
        if (!p) {
            if (!head) return end();
            node_t* del = head;
            head = head->next;
            delete del;
            --sz;
            return iterator(head);
        }
        node_t* del = p->next;
        if (!del) return end();
        p->next = del->next;
        delete del;
        --sz;
        return iterator(p->next);
    }

    iterator erase_after(const_iterator first, const_iterator last) {
        check_iter_pair_(first, last);
        node_t* p = first.ptr;
        node_t* stop = last.ptr;
        if (!p) {
            while (head && head != stop) {
                node_t* del = head;
                head = head->next;
                delete del;
                --sz;
            }
            return iterator(head);
        }
        node_t* cur = p->next;
        while (cur && cur != stop) {
            node_t* nx = cur->next;
            delete cur;
            --sz;
            cur = nx;
        }
        p->next = stop;
        return iterator(stop);
    }

    template<class R>
    void prepend_range(R&& r) {
        ForwardList tmp;
        insert_range_after(tmp.before_begin_internal_(), std::forward<R>(r));
        splice_after(before_begin_internal_(), tmp, tmp.before_begin_internal_(), tmp.end());
    }

    void resize(size_type count) {
        size_type n = sz;
        if (count < n) {
            const_iterator before = before_nth_(count);
            erase_after(before, end());
            return;
        }
        if (count > n) {
            iterator tail_it = last_iter_();
            size_type add = count - n;
            for (size_type i = 0; i < add; ++i) {
                tail_it = insert_after(tail_it, T());
            }
        }
    }

    void resize(size_type count, const T& value) {
        size_type n = sz;
        if (count < n) {
            const_iterator before = before_nth_(count);
            erase_after(before, end());
            return;
        }
        if (count > n) {
            iterator tail_it = last_iter_();
            size_type add = count - n;
            for (size_type i = 0; i < add; ++i) {
                tail_it = insert_after(tail_it, value);
            }
        }
    }

    void swopa(ForwardList& other) noexcept {
        std::swap(head, other.head);
        std::swap(sz, other.sz);
    }

    void merge(ForwardList& other) {
        merge(other, std::less<T>{});
    }

    template<class Comp>
    void merge(ForwardList& other, Comp comp) {
        if (this == &other) return;
        node_t dummy{T{}};
        dummy.next = head;
        node_t* tail = &dummy;
        node_t* a = head;
        node_t* b = other.head;
        while (a && b) {
            if (comp(b->data, a->data)) {
                node_t* nx = b->next;
                tail->next = b;
                tail = b;
                b = nx;
            } else {
                node_t* nx = a->next;
                tail->next = a;
                tail = a;
                a = nx;
            }
        }
        if (a) tail->next = a;
        else tail->next = b;
        head = dummy.next;
        sz += other.sz;
        other.head = nullptr;
        other.sz = 0;
    }

    void splice_after(const_iterator pos, ForwardList& other) {
        if (other.empty()) return;
        const_iterator before_first = other.before_begin_internal_();
        const_iterator before_last  = other.last_iter_();
        splice_after(pos, other, before_first, before_last);
    }

    void splice_after(const_iterator pos, ForwardList& other, const_iterator before_first){
        node_t* first = before_first.ptr ? before_first.ptr->next : other.head;
        if (!first) return;
        const_iterator before_last(first);
        splice_after(pos, other, before_first, before_last);
    }


    void splice_after(const_iterator pos, ForwardList& other, const_iterator before_first, const_iterator before_last) {
        check_iter_single_(pos);
        if (!(other.is_valid_before_(before_first) && other.is_valid_before_(before_last))) {
            throw std::out_of_range("Splice_after.... invalid source iterators");
        }
        if (before_first == before_last) return;
        node_t* first = nullptr;
        if (before_first.ptr) first = before_first.ptr->next;
        else first = other.head;
        node_t* last = nullptr;
        if (before_last.ptr) last = before_last.ptr->next;
        else last = nullptr;
        if (!first) return;
        node_t* tail = first;
        std::size_t moved = 1;
        while (tail->next != last) {
            tail = tail->next;
            ++moved;
        }
        if (before_first.ptr) before_first.ptr->next = last;
        else other.head = last;
        if (pos.ptr) {
            tail->next = pos.ptr->next;
            pos.ptr->next = first;
        } else {
            tail->next = head;
            head = first;
        }
        sz += moved;
        other.sz -= moved;
    }

    void remove(const T& value) {
        remove_if([&](const T& x) { return x == value; });
    }

    template<class Pred>
    void remove_if(Pred pred) {
        node_t dummy{T{}};
        dummy.next = head;
        node_t* cur = &dummy;
        while (cur->next) {
            if (pred(cur->next->data)) {
                node_t* del = cur->next;
                cur->next = del->next;
                delete del;
                --sz;
            } else {
                cur = cur->next;
            }
        }
        head = dummy.next;
    }

    void reverse() {
        node_t* prev = nullptr;
        node_t* cur = head;
        while (cur) {
            node_t* nx = cur->next;
            cur->next = prev;
            prev = cur;
            cur = nx;
        }
        head = prev;
    }

    void unique() {
        unique(std::equal_to<T>{});
    }

    template<class BinPred>
    void unique(BinPred p) {
        if (!head) return;
        node_t* cur = head;
        while (cur->next) {
            if (p(cur->data, cur->next->data)) {
                node_t* del = cur->next;
                cur->next = del->next;
                delete del;
                --sz;
            } else {
                cur = cur->next;
            }
        }
    }

    void sort() {
        sort(std::less<T>{});
    }

    template<class Comp>
    void sort(Comp comp) {
        if (sz < 2) return;
        head = merge_sort_(head, comp);
    }

    iterator before_begin_internal_() noexcept {
        return iterator(nullptr);
    }

    template<class U>
    friend std::ostream& operator<<(std::ostream& os, const ForwardList<U>& flist);

    // задание!!!! Невероятно тяжелое!!!!!!
    void delete_repeated_elements() {
        if (!head) return;
        std::unordered_set<T> hash_table; // под требование O(n) по времени, тк чисто номинально получает элемент за единицу
        node_t* cur = head;               // но там всякие вероятности и большая возможная константа, поэтому я бы писал как обычно
        node_t* prev = nullptr;           // на простом сете
        while (cur) {
            if (hash_table.find(cur->data) != hash_table.end()) {
                prev->next = cur->next;
                delete cur;
                cur = prev->next;
                continue;
            }
            hash_table.insert(cur->data);
            prev = cur;
            cur = cur->next;
        }
    }

private:
    bool contains_node_(node_t* p) const {
        node_t* cur = head;
        while (cur) {
            if (cur == p) return true;
            cur = cur->next;
        }
        return false;
    }

    bool is_valid_before_(const_iterator it) const {
        if (it.ptr == nullptr) return true;
        return contains_node_(it.ptr);
    }

    void check_iter_single_(const_iterator it) const {
        if (!is_valid_before_(it)) {
            throw std::out_of_range(std::string("Iterator..... ne iz etogo lista...."));
        }
    }

    void check_iter_pair_(const_iterator first, const_iterator last) const {
        if (!is_valid_before_(first) || !is_valid_before_(last)) {
            throw std::out_of_range(std::string("Iteratori..... ne iz etogo lista...."));
        }
    }

    iterator last_iter_() noexcept {
        if (head == nullptr) return iterator(nullptr);
        node_t* cur = head;
        while (cur->next) {
            cur = cur->next;
        }
        return iterator(cur);
    }

    const_iterator before_nth_(size_type n) const {
        if (n > sz) throw std::out_of_range("ForwardList before_nth n>size");
        if (n == 0) return const_iterator(nullptr);
        node_t* prev = nullptr;
        node_t* cur = head;
        size_type i = 0;
        while (cur && i < n) {
            prev = cur;
            cur = cur->next;
            ++i;
        }
        return const_iterator(prev);
    }

    template<class Comp>
    static node_t* merge_lists_(node_t* a, node_t* b, Comp comp) {
        node_t dummy{T{}};
        node_t* tail = &dummy;
        while (a && b) {
            if (comp(b->data, a->data)) {
                tail->next = b;
                tail = b;
                b = b->next;
            } else {
                tail->next = a;
                tail = a;
                a = a->next;
            }
        }
        if (a) tail->next = a;
        else tail->next = b;
        return dummy.next;
    }

    template<class Comp>
    static node_t* merge_sort_(node_t* h, Comp comp) {
        if (!h || !h->next) return h;
        node_t* slow = h;
        node_t* fast = h->next;
        while (fast && fast->next) {
            slow = slow->next;
            fast = fast->next->next;
        }
        node_t* mid = slow->next;
        slow->next = nullptr;
        node_t* left = merge_sort_(h, comp);
        node_t* right = merge_sort_(mid, comp);
        return merge_lists_(left, right, comp);
    }
};

template<class T>
bool operator==(const ForwardList<T>& a, const ForwardList<T>& b) {
    auto it1 = a.begin();
    auto it2 = b.begin();
    while (it1 != a.end() && it2 != b.end()) {
        if (!(*it1 == *it2)) return false;
        ++it1;
        ++it2;
    }
    return it1 == a.end() && it2 == b.end();
}

template<class T>
bool operator!=(const ForwardList<T>& a, const ForwardList<T>& b) { return !(a == b); }

template<class T>
bool operator<(const ForwardList<T>& a, const ForwardList<T>& b) {
    return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template<class T>
bool operator>(const ForwardList<T>& a, const ForwardList<T>& b) { return b < a; }

template<class T>
bool operator<=(const ForwardList<T>& a, const ForwardList<T>& b) { return !(b < a); }

template<class T>
bool operator>=(const ForwardList<T>& a, const ForwardList<T>& b) { return !(a < b); }

template<class T>
inline void swopa(ForwardList<T>& x, ForwardList<T>& y) noexcept { x.swopa(y); }

template<class T>
typename ForwardList<T>::size_type erase(ForwardList<T>& c, const T& value) {
    auto old = c.size();
    c.remove(value);
    return old - c.size();
}

template<class T, class Pred>
typename ForwardList<T>::size_type erase_if(ForwardList<T>& c, Pred pred) {
    auto old = c.size();
    c.remove_if(pred);
    return old - c.size();
}
template<class U>
std::ostream& operator<<(std::ostream& os, const ForwardList<U>& flist) {
    bool first = true;
    auto* node = flist.head;
    while (node) {
        if (!first) os << ", ";
        os << node->data;
        first = false;
        node = node->next;
    }
    os << '\n';
    return os;
}
#endif
