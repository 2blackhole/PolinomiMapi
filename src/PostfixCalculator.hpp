#ifndef POSTFIXCALCULATOR_HPP
#define POSTFIXCALCULATOR_HPP

#include "polijop.hpp"
#include "Table.hpp"
#include <string>
#include <vector>
#include <stack>
#include <sstream>
#include <stdexcept>
#include <cctype>

class PostfixCalculator {
public:
    PostfixCalculator() : table_(nullptr) {}

    explicit PostfixCalculator(Table<std::string, polijop>* table) : table_(table) {}

    void SetTable(Table<std::string, polijop>* table) {
        table_ = table;
    }

    polijop Evaluate(const std::string& expression) {
        auto tokens = Tokenize(expression);
        auto postfix = InfixToPostfix(tokens);
        return EvaluatePostfix(postfix);
    }

private:
    Table<std::string, polijop>* table_;

    bool IsNumber(const std::string& str) const {
        if (str.empty()) return false;
        size_t start = (str[0] == '-' || str[0] == '+') ? 1 : 0;
        if (start >= str.length()) return false;
        bool hasDot = false;
        for (size_t i = start; i < str.length(); ++i) {
            if (str[i] == '.') {
                if (hasDot) return false;
                hasDot = true;
            } else if (!std::isdigit(str[i])) {
                return false;
            }
        }
        return true;
    }

    bool IsOperator(const std::string& str) const {
        return str == "+" || str == "-" || str == "*" || str == "/" ||
               str == "diff" || str == "int";
    }

    double StringToNumber(const std::string& str) const {
        std::istringstream iss(str);
        double value;
        iss >> value;
        return value;
    }

    polijop CreateConstantPolinom(double value) const {
        return polijop(std::vector<std::pair<int32_t, double>>{{0, value}});
    }

    int GetPriority(const std::string& op) const {
        if (op == "+" || op == "-") return 1;
        if (op == "*" || op == "/") return 2;
        if (op == "diff" || op == "int") return 4;
        return 0;
    }

    bool IsLeftAssociative(const std::string& op) const {
        return op != "^";
    }

    std::vector<std::string> Tokenize(const std::string& expr) {
        std::vector<std::string> tokens;
        std::string current;

        for (size_t i = 0; i < expr.length(); ++i) {
            char c = expr[i];

            if (std::isspace(c)) {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
            } else if (c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')') {
                if (!current.empty()) {
                    tokens.push_back(current);
                    current.clear();
                }
                tokens.push_back(std::string(1, c));
            } else {
                current += c;
            }
        }

        if (!current.empty()) {
            tokens.push_back(current);
        }

        return tokens;
    }

    std::vector<std::string> InfixToPostfix(const std::vector<std::string>& tokens) {
        std::vector<std::string> output;
        std::stack<std::string> operators;

        for (const std::string& token : tokens) {
            if (IsNumber(token) || (!IsOperator(token) && token != "(" && token != ")")) {
                output.push_back(token);
            } else if (IsOperator(token)) {
                while (!operators.empty() &&
                       operators.top() != "(" &&
                       ((IsLeftAssociative(token) && GetPriority(token) <= GetPriority(operators.top())) ||
                        (!IsLeftAssociative(token) && GetPriority(token) < GetPriority(operators.top())))) {
                    output.push_back(operators.top());
                    operators.pop();
                }
                operators.push(token);
            } else if (token == "(") {
                operators.push(token);
            } else if (token == ")") {
                while (!operators.empty() && operators.top() != "(") {
                    output.push_back(operators.top());
                    operators.pop();
                }
                if (!operators.empty()) {
                    operators.pop();
                }
            }
        }

        while (!operators.empty()) {
            output.push_back(operators.top());
            operators.pop();
        }

        return output;
    }

    polijop EvaluatePostfix(const std::vector<std::string>& postfix) {
        std::stack<polijop> stack;

        for (const std::string& token : postfix) {
            if (IsNumber(token)) {
                double value = StringToNumber(token);
                stack.push(CreateConstantPolinom(value));
            } else if (IsOperator(token)) {
                if (token == "diff" || token == "int") {
                    if (stack.empty()) {
                        throw std::runtime_error("Not enough operands for unary operator: " + token);
                    }
                    polijop operand = stack.top();
                    stack.pop();
                    if (token == "diff") {
                        stack.push(operand.differentiate(0));
                    } else {
                        stack.push(operand.integrate(0));
                    }
                } else {
                    if (stack.size() < 2) {
                        throw std::runtime_error("Not enough operands for binary operator: " + token);
                    }
                    polijop right = stack.top(); stack.pop();
                    polijop left = stack.top(); stack.pop();

                    if (token == "+") {
                        stack.push(left + right);
                    } else if (token == "-") {
                        stack.push(left - right);
                    } else if (token == "*") {
                        stack.push(left * right);
                    } else if (token == "/") {
                        if (right.getMonomCount() == 1 &&
                            right.getMonoms().front().getI() == 0 &&
                            right.getMonoms().front().getJ() == 0 &&
                            right.getMonoms().front().getK() == 0) {
                            double divisor = right.getMonoms().front().a;
                            if (std::abs(divisor) < 1e-9) {
                                throw std::runtime_error("Division by zero");
                            }
                            stack.push(left * (1.0 / divisor));
                        } else {
                            throw std::runtime_error("Division only supported for constants");
                        }
                    }
                }
            } else {
                if (!table_) {
                    throw std::runtime_error("No table set for polynomial lookup");
                }

                polijop* found = table_->get(token);
                if (!found) {
                    throw std::runtime_error("Polynomial not found: " + token);
                }
                stack.push(*found);
            }
        }

        if (stack.size() != 1) {
            throw std::runtime_error("Invalid expression: stack size is " + std::to_string(stack.size()));
        }

        return stack.top();
    }
};

#endif