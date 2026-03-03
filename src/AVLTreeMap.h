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
        Node* p = nullptr;
        Node* right = nullptr;
        Node* left = nullptr;

        TKey key;
        TVal val;

        explicit Node(const Pair& p) {
            key = p.first;
            val = p.second;
        }
    };

};

#endif //AVLTREEMAP_H
