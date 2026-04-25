#pragma once
#include <vector>
#include <utility>

template<typename Key, typename Value>
class Table {
public:
    virtual ~Table() = default;

    virtual void add(const Key& key, const Value& value) = 0;
    virtual bool remove(const Key& key) = 0;
    virtual Value* get(const Key& key) = 0;
    virtual void clear() = 0;
    virtual size_t size() = 0;
    virtual bool empty() = 0;
    virtual std::vector<Key> keys() = 0;
    virtual std::vector<std::pair<Key, Value>> items() = 0;
    virtual bool contains(const Key& key) = 0;
};