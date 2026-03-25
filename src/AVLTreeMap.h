//
// Created by banani on 03/03/26.
//

#ifndef AVLTREEMAP_H
#define AVLTREEMAP_H
#include <bits/stdc++.h>
using std::pair;

template<class TKey, class TVal>
class AVLTreeMap {
    using Pair = pair<TKey, TVal>;

    struct Node {
        Node* parent = nullptr;
        Node* right = nullptr;
        Node* left = nullptr;

        Pair data;

        int height;

        explicit Node(const Pair& p) {
            data.first = p.first;
            data.second = p.second;
            height = 1;
        }
    };

    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Pair;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;
        using node_type = Node;

    private:
        node_type* node;

    public:
        Iterator(node_type* n = nullptr) : node(n) {}

        reference operator*() const { return node->data; }
        pointer operator->() const { return &(operator*()); }
        
        Iterator& operator++() {
            node = increment(node);
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }
        Iterator& operator--() {
            node = decrement(node);
            return *this;
        }
        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }
        bool operator==(const Iterator& other) const { return node == other.node; }
        bool operator!=(const Iterator& other) const { return node != other.node; }

    private:
        static node_type* increment(node_type* x) {
            if (!x) return nullptr;
            if (x->right) {
                x = x->right;
                while (x->left) x = x->left;
                return x;
            }
            node_type* y = x->parent;
            while (y && x == y->right) {
                x = y;
                y = y->parent;
            }
            return y;
        }

        static node_type* decrement(node_type* x) {
            if (!x) return nullptr;
            if (x->left) {
                x = x->left;
                while (x->right) x = x->right;
                return x;
            }
            node_type* y = x->parent;
            while (y && x == y->left) {
                x = y;
                y = y->parent;
            }
            return y;
        }

        friend class AVLTreeMap;
    };

    Node* root = nullptr;
    int sz = 0;

    //L-R
    static int diff(const Node& x) {
        if (x.left == nullptr && x.right == nullptr) {
            return 0;
        }
        if (x.left == nullptr) {
            return -1 * x.right->height;
        }
        if (x.right == nullptr) {
            return x.left->height;
        }
        return x.left->height - x.right->height;
    }

    Node* find(Node* v, const TKey& k) {
        if (v == nullptr) return nullptr;
        if (k == v->data.first) return v;
        if (k < v->data.first) return find(v->left, k);
        return find(v->right, k);
    }

    void update_height(Node* node) {
        int left_h = node->left ? node->left->height : 0;
        int right_h = node->right ? node->right->height : 0;
        node->height = 1 + std::max(left_h, right_h);
    }

    void rotate_left(Node* v) {
        Node* b = v->right;
        if (!b) return;
        
        v->right = b->left;
        if (b->left) b->left->parent = v;
        
        b->left = v;
        Node* ded = v->parent;
        b->parent = ded;
        v->parent = b;

        if (ded) {
            if (ded->left == v) ded->left = b;
            else ded->right = b;
        } else {
            root = b;
        }

        update_height(v);
        update_height(b);
    }

    void rotate_right(Node* v) {
        Node* b = v->left;
        if (!b) return;

        v->left = b->right;
        if (b->right) b->right->parent = v;

        b->right = v;
        Node* ded = v->parent;
        b->parent = ded;
        v->parent = b;

        if (ded) {
            if (ded->left == v) ded->left = b;
            else ded->right = b;
        } else {
            root = b;
        }

        update_height(v);
        update_height(b);
    }

    void big_rotate_left(Node* v) {
        rotate_right(v->right);
        rotate_left(v);
    }

    void big_rotate_right(Node* v) {
        rotate_left(v->left);
        rotate_right(v);
    }

    void balance(Node* v) {
        while (v) {
            update_height(v);
            int factor = diff(*v);

            if (factor > 1) {
                if (diff(*v->left) >= 0)
                    rotate_right(v);
                else
                    big_rotate_right(v);
            } else if (factor < -1) {
                if (diff(*v->right) <= 0)
                    rotate_left(v);
                else
                    big_rotate_left(v);
            }
            v = v->parent;
        }
    }

    void swap(AVLTreeMap& other) noexcept {
        std::swap(root, other.root);
        std::swap(sz, other.sz);
    }

    Node* get_min(Node* v) {
        if (!v) return nullptr;
        while (v->left) v = v->left;
        return v;
    }

    Node* _insert(const Pair& p) {
        Node* c = root;
        Node* otec = nullptr;

        while (c) {
            otec = c;
            if (p.first == c->data.first) {
                return c;
            } else if (p.first < c->data.first) {
                c = c->left;
            } else {
                c = c->right;
            }
        }

        Node* b = new Node(p);
        b->parent = otec;

        if (!otec) {
            root = b;
        } else if (p.first < otec->data.first) {
            otec->left = b;
        } else {
            otec->right = b;
        }

        ++sz;
        if (otec) balance(otec);
        return b;
    }

    void erase(Node* v) {
        if (!v) return;

        if (v->left && v->right) {
            Node* succ = get_min(v->right);
            v->data = succ->data;
            erase(succ);
            return;
        }

        Node* child = v->left ? v->left : v->right;
        Node* parent = v->parent;

        if (child) child->parent = parent;

        if (!parent) {
            root = child;
        } else {
            if (parent->left == v) parent->left = child;
            else parent->right = child;
        }

        delete v;
        --sz;

        if (parent) balance(parent);
        else if (child) balance(child);
    }

    Node* copy_tree(Node* other, Node* p) {
        if (!other) return nullptr;
        Node* b = new Node(other->data);
        b->height = other->height;
        b->parent = p;
        b->left = copy_tree(other->left, b);
        b->right = copy_tree(other->right, b);
        return b;
    }

    void clear(Node* v) {
        if (!v) return;
        clear(v->left);
        clear(v->right);
        delete v;
    }

public:
    AVLTreeMap() : root(nullptr), sz(0) {}

    AVLTreeMap(const AVLTreeMap& other) : root(nullptr), sz(other.sz) {
        if (other.root) {
            root = copy_tree(other.root, nullptr);
        }
    }

    AVLTreeMap(AVLTreeMap&& other) noexcept
        : root(other.root), sz(other.sz) {
        other.root = nullptr;
        other.sz = 0;
    }

    AVLTreeMap& operator=(const AVLTreeMap& other) {
        if (this != &other) {
            AVLTreeMap tmp(other);
            swap(tmp);
        }
        return *this;
    }

    AVLTreeMap& operator=(AVLTreeMap&& other) noexcept {
        if (this != &other) {
            clear(root);
            root = other.root;
            sz = other.sz;
            other.root = nullptr;
            other.sz = 0;
        }
        return *this;
    }

    ~AVLTreeMap() {
        clear(root);
    }

    Iterator begin() {
        Node* cur = root;
        if (cur) while (cur->left) cur = cur->left;
        return Iterator(cur);
    }

    Iterator end() {
        return Iterator(nullptr);
    }

    Iterator find(const TKey& k) {
        return Iterator(find(root, k));
    }

    TVal& operator[](const TKey& k) {
        Iterator it = find(k);
        if (it == end()) {
            Node* v = _insert({k, TVal()});
            return v->data.second;
        }
        return it->second;
    }

    pair<Iterator, bool> insert(const Pair& p) {
        if (find(p.first) == end()) {
            return {Iterator(_insert(p)), true};
        }
        return {find(p.first), false};
    }

    bool empty() {return sz == 0;}

    size_t size() {return sz;}

    Iterator erase(Iterator pos) {
        if (pos == end()) return end();
        Node* node = pos.node;
        Node* next = Iterator::increment(node);
        erase(node);
        return Iterator(next);
    }

    size_t erase(const TKey& k) {
        Node* node = find(root, k);
        if (!node) return 0;
        erase(node);
        return 1;
    }


};

#endif //AVLTREEMAP_H
