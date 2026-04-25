#ifndef MONON_HPP
#define MONON_HPP

#include <cstdint>
#include <cmath>
#include <stdexcept>
#include <iostream>

const double EPS = 1e-9;

struct monon {
    int32_t ijk = 0;
    double a = 0;

    monon(int f, double s) {
        ijk = f; a = s;
    }
    
    monon() = default;

    friend monon operator+(const monon& a, const monon& b) {
        if (a.ijk != b.ijk) throw std::invalid_argument("vse ploho!");
        return {a.ijk, a.a + b.a};
    }

    friend bool operator==(const monon &a, const monon &b) {
        if (a.ijk == b.ijk && std::abs(a.a - b.a) < EPS) return true;
        return false;
    }

    friend bool operator!=(const monon &a, const monon &b) {
        return !(a == b);
    }

    friend monon operator*(const monon& a, const monon& b) {
        int32_t mask = (1 << 10) - 1;
        int32_t i_a = a.ijk & mask, i_b = b.ijk & mask;
        int32_t j_a = (a.ijk >> 10) & mask, j_b = (b.ijk >> 10) & mask;
        int32_t k_a = (a.ijk >> 20) & mask, k_b = (b.ijk >> 20) & mask;
        int32_t i = i_a + i_b;
        int32_t j = j_a + j_b;
        int32_t k = k_a + k_b;
        if (i >= 1024 || j >= 1024 || k >= 1024) throw std::overflow_error("Deg is way big...");
        int32_t ijk = k; ijk <<= 10; ijk += j; ijk <<= 10; ijk += i;
        return monon(ijk, a.a * b.a);
    }
    
    bool isZero() const { return std::abs(a) < EPS; }
    
    int getI() const { return ijk & 0x3FF; }
    int getJ() const { return (ijk >> 10) & 0x3FF; }
    int getK() const { return (ijk >> 20) & 0x3FF; }
    
    static int32_t pack(int i, int j, int k) {
        if (i >= 1024 || j >= 1024 || k >= 1024)
            throw std::overflow_error("Degree too large");
        return (k << 20) | (j << 10) | i;
    }
    
    monon differentiate(int var) const {
        int power = (var == 0) ? getI() : (var == 1) ? getJ() : getK();
        if (power == 0) return monon(0, 0.0);
        int new_i = getI(), new_j = getJ(), new_k = getK();
        double new_a = a * power;
        if (var == 0) new_i--;
        else if (var == 1) new_j--;
        else new_k--;
        return monon(pack(new_i, new_j, new_k), new_a);
    }
    
    monon integrate(int var) const {
        int power = (var == 0) ? getI() : (var == 1) ? getJ() : getK();
        int new_i = getI(), new_j = getJ(), new_k = getK();
        if (var == 0) new_i++;
        else if (var == 1) new_j++;
        else new_k++;
        if (power + 1 == 0)
            throw std::invalid_argument("Cannot integrate x^{-1}");
        double new_a = a / (power + 1);
        return monon(pack(new_i, new_j, new_k), new_a);
    }
    
    double evaluate(double x, double y, double z) const {
        double res = a;
        int i = getI(), j = getJ(), k = getK();
        if (i > 0) res *= std::pow(x, i);
        if (j > 0) res *= std::pow(y, j);
        if (k > 0) res *= std::pow(z, k);
        return res;
    }
};

#endif