#ifndef TABLETYPE_HPP
#define TABLETYPE_HPP

#include <string>

enum class TableType {
    UnorderedVector,
    OrderedVector,
    AVLTree,
    RedBlackTree,
    ChainHash,
    OpenAddrHash
};

inline std::string tableTypeName(TableType t) {
    switch (t) {
        case TableType::UnorderedVector: return "Unordered Vector";
        case TableType::OrderedVector:   return "Ordered Vector";
        case TableType::AVLTree:         return "AVL Tree";
        case TableType::RedBlackTree:    return "Red-Black Tree";
        case TableType::ChainHash:       return "Chain Hash";
        case TableType::OpenAddrHash:    return "Open Addressing Hash";
        default: return "Unknown";
    }
}

#endif