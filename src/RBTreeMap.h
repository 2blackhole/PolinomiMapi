#pragma once
#include <utility>
#include <stdexcept>
#include <cstddef>

enum Color { RED, BLACK };

template <typename TKey, typename TVal>
class RBTreeMap {
public:
    using Pair = std::pair<const TKey, TVal>;

private:
    struct Node {
        Pair data;
        Color color;
        Node* left;
        Node* right;
        Node* parent;

        Node(const TKey& key, const TVal& val, Color c = RED,
             Node* l = nullptr, Node* r = nullptr, Node* p = nullptr)
            : data(key, val), color(c), left(l), right(r), parent(p) {}
    };

    static Node _nil;
    static Node* const NIL;

    Node* root;
    size_t sz;

    static void initNil() {
        if (_nil.color != BLACK) {
            _nil.color = BLACK;
            _nil.left = _nil.right = _nil.parent = &_nil;
        }
    }
    static Node* minimum(Node* x) {
        if (x == NIL) return NIL;
        while (x->left != NIL) x = x->left;
        return x;
    }

    static Node* maximum(Node* x) {
        if (x == NIL) return NIL;
        while (x->right != NIL) x = x->right;
        return x;
    }

    static Node* increment(Node* x) {
        if (x == NIL) return NIL;
        if (x->right != NIL) {
            x = x->right;
            while (x->left != NIL) x = x->left;
        } else {
            Node* y = x->parent;
            while (y != NIL && x == y->right) {
                x = y;
                y = y->parent;
            }
            x = y;
        }
        return x;
    }

    static Node* decrement(Node* x) {
        if (x == NIL) return maximum(root);
        if (x->left != NIL) {
            x = x->left;
            while (x->right != NIL) x = x->right;
        } else {
            Node* y = x->parent;
            while (y != NIL && x == y->left) {
                x = y;
                y = y->parent;
            }
            x = y;
        }
        return x;
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
                Node* brother = parent->right;
                if (brother->color == RED) {
                    brother->color = BLACK;
                    parent->color = RED;
                    leftRotate(parent);
                    brother = parent->right;
                }
                if (brother->left->color == BLACK && brother->right->color == BLACK) {
                    brother->color = RED;
                    x = parent;
                    parent = x->parent;
                    if (parent != NIL) isLeft = (x == parent->left);
                } else {
                    if (brother->right->color == BLACK) {
                        brother->left->color = BLACK;
                        brother->color = RED;
                        rightRotate(brother);
                        brother = parent->right;
                    }
                    brother->color = parent->color;
                    parent->color = BLACK;
                    brother->right->color = BLACK;
                    leftRotate(parent);
                    x = root;
                }
            } else {
                Node* brother = parent->left;
                if (brother->color == RED) {
                    brother->color = BLACK;
                    parent->color = RED;
                    rightRotate(parent);
                    brother = parent->left;
                }
                if (brother->right->color == BLACK && brother->left->color == BLACK) {
                    brother->color = RED;
                    x = parent;
                    parent = x->parent;
                    if (parent != NIL) isLeft = (x == parent->left);
                } else {
                    if (brother->left->color == BLACK) {
                        brother->right->color = BLACK;
                        brother->color = RED;
                        leftRotate(brother);
                        brother = parent->left;
                    }
                    brother->color = parent->color;
                    parent->color = BLACK;
                    brother->left->color = BLACK;
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
        v->parent = u->parent;
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
        using node_type = Node;

    private:
        node_type* node;

    public:
        explicit Iterator(node_type* n = nullptr) : node(n) {}

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
            if (node == nullptr)
                node = maximum(root);
            else
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

        friend class RBTreeMap;
    };

    RBTreeMap() : root(NIL), sz(0) {
        initNil();
    }

    ~RBTreeMap() { clear(root); }

    RBTreeMap(const RBTreeMap& other) : root(NIL), sz(other.sz) {
        initNil();
        if (other.root != NIL) root = copyTree(other.root);
    }

    RBTreeMap(RBTreeMap&& other) noexcept
        : root(other.root), sz(other.sz) {
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

    bool empty() const { return sz == 0; }
    size_t size() const { return sz; }

    TVal& operator[](const TKey& key) {
        Iterator it = find(key);
        if (it == end()) {
            it = insert({key, TVal{}}).first;
        }
        return it->second;
    }

    Iterator find(const TKey& key) {
        Node* n = findNode(key);
        return Iterator(n == NIL ? nullptr : n);
    }

    std::pair<Iterator, bool> insert(const Pair& p) {
        const TKey& key = p.first;
        Node* cur = root;
        Node* parent = NIL;
        bool dup = false;

        while (cur != NIL) {
            parent = cur;
            if (key < cur->data.first)
                cur = cur->left;
            else if (cur->data.first < key)
                cur = cur->right;
            else {
                dup = true;
                break;
            }
        }

        if (dup) {
            cur->data.second = p.second;
            return {Iterator(cur), false};
        }

        Node* newNode = new Node(key, p.second, RED, NIL, NIL, NIL);
        newNode->parent = parent;
        if (parent == NIL) {
            root = newNode;
        } else if (key < parent->data.first) {
            parent->left = newNode;
        } else {
            parent->right = newNode;
        }

        fixInsertion(newNode);
        ++sz;
        return {Iterator(newNode), true};
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
        return Iterator(minNode == NIL ? nullptr : minNode);
    }

    Iterator end() {
        return Iterator(nullptr);
    }
};

template <typename TKey, typename TVal>
typename RBTreeMap<TKey, TVal>::Node RBTreeMap<TKey, TVal>::_nil;

template <typename TKey, typename TVal>
typename RBTreeMap<TKey, TVal>::Node* const RBTreeMap<TKey, TVal>::NIL = &_nil;