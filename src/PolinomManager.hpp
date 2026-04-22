#pragma once
#include "PostfixCalculator.hpp"
#include "TableType.hpp"
#include "Table.hpp"
#include "AVLTreeMap.h"
#include "RBTreeMap.h"
#include "ChainHashTableMap.h"
#include "OpenAddressingHashTableMap.h"
#include "OrderedMapVec.h"
#include "UnorderedMapVec.h"
#include <memory>
#include <vector>
#include <string>
#include "polijop.hpp"
#include <stdexcept>

class PolinomManager {
public:
    using TablePtr = std::unique_ptr<Table<std::string, polijop>>;

    PolinomManager() {
        table_ = std::make_unique<UnorderedMapVec<std::string, polijop>>();
        tableType_ = TableType::UnorderedVector;
        calc_.SetTable(table_.get());
    }

    void SetActiveTable(TableType type) {
        if (type == tableType_) return;

        TablePtr newTable = createTable(type);
        for (const auto& item : table_->items()) {
            newTable->add(item.first, item.second);
        }

        table_ = std::move(newTable);
        tableType_ = type;
        calc_.SetTable(table_.get());
    }

    TableType GetActiveTableType() const {
        return tableType_;
    }

    std::string GetActiveTableName() const {
        return tableTypeName(tableType_);
    }

    Table<std::string, polijop>* GetActiveTable() {
        return table_.get();
    }

    bool AddPolinom(const std::string& name, const polijop& p) {
        if (table_->contains(name)) return false;
        table_->add(name, p);
        return true;
    }

    bool RemovePolinom(const std::string& name) {
        return table_->remove(name);
    }

    polijop* FindPolinom(const std::string& name) {
        return table_->get(name);
    }

    bool Contains(const std::string& name) {
        return table_->contains(name);
    }

    std::vector<std::pair<std::string, polijop>> GetAll() {
        return table_->items();
    }

    std::vector<std::string> GetNames() {
        return table_->keys();
    }

    size_t GetPolinomCount() {
        return table_->size();
    }

    polijop EvaluateExpression(const std::string& expr) {
        return calc_.Evaluate(expr);
    }

    bool EvaluateAndStore(const std::string& name, const std::string& expr) {
        try {
            polijop res = calc_.Evaluate(expr);
            return AddPolinom(name, res);
        } catch (...) {
            return false;
        }
    }

    polijop Differentiate(const std::string& name, int var) {
        polijop* p = FindPolinom(name);
        if (!p) throw std::runtime_error("Polynomial not found");
        return p->differentiate(var);
    }

    polijop Integrate(const std::string& name, int var) {
        polijop* p = FindPolinom(name);
        if (!p) throw std::runtime_error("Polynomial not found");
        return p->integrate(var);
    }

    polijop MultiplyByConstant(const std::string& name, double c) {
        polijop* p = FindPolinom(name);
        if (!p) throw std::runtime_error("Polynomial not found");
        return (*p) * c;
    }

    double EvaluateAt(const std::string& name, double x, double y, double z) {
        polijop* p = FindPolinom(name);
        if (!p) throw std::runtime_error("Polynomial not found");
        return p->evaluate(x, y, z);
    }

    void ClearAll() {
        table_->clear();
    }

    void CreateSamplePolinoms() {
        AddPolinom("p1", polijop(std::vector<std::pair<int32_t, double>>{
            {monon::pack(2,0,0), 3.0},
            {monon::pack(0,1,0), -2.0},
            {monon::pack(0,0,0), 5.0}
        }));
        AddPolinom("p2", polijop(std::vector<std::pair<int32_t, double>>{
            {monon::pack(1,1,0), 1.0},
            {monon::pack(0,0,1), 4.0}
        }));
        AddPolinom("zero", polijop(std::vector<std::pair<int32_t, double>>{
            {0, 0.0}
        }));
    }

private:
    TablePtr createTable(TableType type) {
        switch (type) {
            case TableType::UnorderedVector:
                return std::make_unique<UnorderedMapVec<std::string, polijop>>();
            case TableType::OrderedVector:
                return std::make_unique<OrderedMapVec<std::string, polijop>>();
            case TableType::AVLTree:
                return std::make_unique<AVLTreeMap<std::string, polijop>>();
            case TableType::RedBlackTree:
                return std::make_unique<RBTreeMap<std::string, polijop>>();
            case TableType::ChainHash:
                return std::make_unique<ChainHashTableMap<std::string, polijop>>();
            case TableType::OpenAddrHash:
                return std::make_unique<OpenAddressingHashTableMap<std::string, polijop>>();
            default:
                throw std::invalid_argument("Unknown table type");
        }
    }

    TablePtr table_;
    TableType tableType_;
    PostfixCalculator calc_;
};