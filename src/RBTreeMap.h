#pragma once
#include <utility>
#include <cstddef>
#include "Table.hpp"

enum Color { RED, BLACK };

template <typename TKey, typename TVal>
class RBTreeMap : public Table<TKey, TVal> {
public:
    using Pair = std::pair<const TKey, TVal>;

private:
    struct Node {
        Pair data;
        Color color;
        Node* left;
        Node* right;
        Node* parent;

        Node() : color(BLACK), left(nullptr), right(nullptr), parent(nullptr) {}
        Node(const TKey& key, const TVal& val, Color c = RED,
             Node* l = nullptr, Node* r = nullptr, Node* p = nullptr)
            : data(key, val), color(c), left(l), right(r), parent(p) {}
    };

    static Node _nil;
    static Node* const NIL;

    Node* root;
    size_t sz;

    static void initNil() {
        _nil.color = BLACK;
        _nil.left = _nil.right = _nil.parent = NIL;
    }

    Node* minimum(Node* x) const {
        if (x == NIL) return NIL;
        while (x->left != NIL) x = x->left;
        return x;
    }

    Node* maximum(Node* x) const {
        if (x == NIL) return NIL;
        while (x->right != NIL) x = x->right;
        return x;
    }

    Node* increment(Node* x) const {
        if (x == NIL) return NIL;
        if (x->right != NIL) {
            x = x->right;
            while (x->left != NIL) x = x->left;
            return x;
        }
        Node* y = x->parent;
        while (y != NIL && x == y->right) {
            x = y;
            y = y->parent;
        }
        return y;
    }

    Node* decrement(Node* x) const {
        if (x == NIL) return maximum(root);
        if (x->left != NIL) {
            x = x->left;
            while (x->right != NIL) x = x->right;
            return x;
        }
        Node* y = x->parent;
        while (y != NIL && x == y->left) {
            x = y;
            y = y->parent;
        }
        return y;
    }

    Node* findNode(const TKey& key) const {
        Node* cur = root;
        while (cur != NIL) {
            if (key < cur->data.first)
                cur = cur->left;
            else if (cur->data.first < key)
                cur = cur->right;
            else
                return cur;
        }
        return NIL;
    }

    void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left != NIL) y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == NIL)
            root = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;
        y->left = x;
        x->parent = y;
    }

    void rightRotate(Node* y) {
        Node* x = y->left;
        y->left = x->right;
        if (x->right != NIL) x->right->parent = y;
        x->parent = y->parent;
        if (y->parent == NIL)
            root = x;
        else if (y == y->parent->left)
            y->parent->left = x;
        else
            y->parent->right = x;
        x->right = y;
        y->parent = x;
    }

    void fixInsertion(Node* t) {
        while (t != root && t->parent->color == RED) {
            Node* parent = t->parent;
            Node* grandparent = parent->parent;
            if (parent == grandparent->left) {
                Node* uncle = grandparent->right;
                if (uncle->color == RED) {
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    grandparent->color = RED;
                    t = grandparent;
                } else {
                    if (t == parent->right) {
                        t = parent;
                        leftRotate(t);
                        parent = t->parent;
                    }
                    parent->color = BLACK;
                    grandparent->color = RED;
                    rightRotate(grandparent);
                }
            } else {
                Node* uncle = grandparent->left;
                if (uncle->color == RED) {
                    parent->color = BLACK;
                    uncle->color = BLACK;
                    grandparent->color = RED;
                    t = grandparent;
                } else {
                    if (t == parent->left) {
                        t = parent;
                        rightRotate(t);
                        parent = t->parent;
                    }
                    parent->color = BLACK;
                    grandparent->color = RED;
                    leftRotate(grandparent);
                }
            }
        }
        root->color = BLACK;
    }

    void fixDeleting(Node* x, Node* parent, bool isLeft) {
        while (x != root && (x == NIL || x->color == BLACK)) {
            if (isLeft) {
                Node* w = parent->right;
                if (w->color == RED) {
                    w->color = BLACK;
                    parent->color = RED;
                    leftRotate(parent);
                    w = parent->right;
                }
                if ((w->left == NIL || w->left->color == BLACK) &&
                    (w->right == NIL || w->right->color == BLACK)) {
                    if (w != NIL) w->color = RED;
                    x = parent;
                    parent = x->parent;
                    if (parent != NIL) isLeft = (x == parent->left);
                } else {
                    if (w->right == NIL || w->right->color == BLACK) {
                        if (w->left != NIL) w->left->color = BLACK;
                        if (w != NIL) w->color = RED;
                        rightRotate(w);
                        w = parent->right;
                    }
                    if (w != NIL) w->color = parent->color;
                    parent->color = BLACK;
                    if (w->right != NIL) w->right->color = BLACK;
                    leftRotate(parent);
                    x = root;
                }
            } else {
                Node* w = parent->left;
                if (w->color == RED) {
                    w->color = BLACK;
                    parent->color = RED;
                    rightRotate(parent);
                    w = parent->left;
                }
                if ((w->right == NIL || w->right->color == BLACK) &&
                    (w->left == NIL || w->left->color == BLACK)) {
                    if (w != NIL) w->color = RED;
                    x = parent;
                    parent = x->parent;
                    if (parent != NIL) isLeft = (x == parent->left);
                } else {
                    if (w->left == NIL || w->left->color == BLACK) {
                        if (w->right != NIL) w->right->color = BLACK;
                        if (w != NIL) w->color = RED;
                        leftRotate(w);
                        w = parent->left;
                    }
                    if (w != NIL) w->color = parent->color;
                    parent->color = BLACK;
                    if (w->left != NIL) w->left->color = BLACK;
                    rightRotate(parent);
                    x = root;
                }
            }
        }
        if (x != NIL) x->color = BLACK;
        root->color = BLACK;
    }

    void transplant(Node* u, Node* v) {
        if (u->parent == NIL)
            root = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;
        if (v != NIL) v->parent = u->parent;
    }

    Node* copyTree(Node* node) const {
        if (node == NIL) return NIL;
        Node* newNode = new Node(node->data.first, node->data.second, node->color, NIL, NIL, NIL);
        newNode->left = copyTree(node->left);
        if (newNode->left != NIL) newNode->left->parent = newNode;
        newNode->right = copyTree(node->right);
        if (newNode->right != NIL) newNode->right->parent = newNode;
        return newNode;
    }

    void clear(Node* node) {
        if (node == NIL) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

public:
    class Iterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = Pair;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

    private:
        Node* node;
        const RBTreeMap* tree;

    public:
        explicit Iterator(Node* n = NIL, const RBTreeMap* t = nullptr) : node(n), tree(t) {}

        reference operator*() const { return node->data; }
        pointer operator->() const { return &(operator*()); }

        Iterator& operator++() {
            node = tree->increment(node);
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        Iterator& operator--() {
            if (node == NIL)
                node = tree->maximum(tree->root);
            else
                node = tree->decrement(node);
            return *this;
        }
        Iterator operator--(int) {
            Iterator tmp = *this;
            --(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const { return node == other.node; }
        bool operator!=(const Iterator& other) const { return node != other.node; }

        friend class RBTreeMap;
    };

    RBTreeMap() : root(NIL), sz(0) { initNil(); }
    ~RBTreeMap() { clear(root); }

    RBTreeMap(const RBTreeMap& other) : root(NIL), sz(other.sz) {
        if (other.root != NIL) root = copyTree(other.root);
    }

    RBTreeMap(RBTreeMap&& other) noexcept : root(other.root), sz(other.sz) {
        other.root = NIL;
        other.sz = 0;
    }

    RBTreeMap& operator=(const RBTreeMap& other) {
        if (this != &other) {
            clear(root);
            sz = other.sz;
            if (other.root != NIL)
                root = copyTree(other.root);
            else
                root = NIL;
        }
        return *this;
    }

    RBTreeMap& operator=(RBTreeMap&& other) noexcept {
        if (this != &other) {
            clear(root);
            root = other.root;
            sz = other.sz;
            other.root = NIL;
            other.sz = 0;
        }
        return *this;
    }

    TVal& operator[](const TKey& key) {
        Iterator it = find(key);
        if (it == end()) {
            it = insert({key, TVal{}}).first;
        }
        return it->second;
    }

    Iterator find(const TKey& key) {
        Node* n = findNode(key);
        return Iterator(n == NIL ? NIL : n, this);
    }

    std::pair<Iterator, bool> insert(const Pair& p) {
        const TKey& key = p.first;
        Node* cur = root;
        Node* parent = NIL;

        while (cur != NIL) {
            parent = cur;
            if (key < cur->data.first)
                cur = cur->left;
            else if (cur->data.first < key)
                cur = cur->right;
            else
                return {Iterator(cur, this), false};
        }

        Node* newNode = new Node(key, p.second, RED, NIL, NIL, NIL);
        newNode->parent = parent;
        if (parent == NIL)
            root = newNode;
        else if (key < parent->data.first)
            parent->left = newNode;
        else
            parent->right = newNode;

        fixInsertion(newNode);
        ++sz;
        return {Iterator(newNode, this), true};
    }

    Iterator erase(Iterator pos) {
        if (pos == end()) return end();
        Node* z = pos.node;
        Iterator next = pos;
        ++next;

        Node* y = z;
        Node* x = NIL;
        Node* x_parent = NIL;
        bool x_is_left = false;
        Color y_original_color = y->color;

        if (z->left == NIL) {
            x = z->right;
            x_parent = z->parent;
            if (x_parent != NIL) x_is_left = (z == x_parent->left);
            transplant(z, z->right);
        } else if (z->right == NIL) {
            x = z->left;
            x_parent = z->parent;
            if (x_parent != NIL) x_is_left = (z == x_parent->left);
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            y_original_color = y->color;
            x = y->right;
            if (y->parent == z) {
                x_parent = y;
                x_is_left = false;
            } else {
                x_parent = y->parent;
                x_is_left = (x == x_parent->left);
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        delete z;
        --sz;

        if (y_original_color == BLACK) {
            fixDeleting(x, x_parent, x_is_left);
        }
        return next;
    }

    size_t erase(const TKey& key) {
        Iterator it = find(key);
        if (it == end()) return 0;
        erase(it);
        return 1;
    }

    Iterator begin() {
        Node* minNode = minimum(root);
        return Iterator(minNode, this);
    }

    Iterator end() {
        return Iterator(NIL, this);
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
        return sz;
    }

    bool empty() override {
        return sz == 0;
    }

    void clear() override {
        clear(root);
        root = NIL;
        sz = 0;
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

template <typename TKey, typename TVal>
typename RBTreeMap<TKey, TVal>::Node RBTreeMap<TKey, TVal>::_nil;

template <typename TKey, typename TVal>
typename RBTreeMap<TKey, TVal>::Node* const RBTreeMap<TKey, TVal>::NIL = &_nil;