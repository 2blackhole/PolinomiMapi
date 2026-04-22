//
// Created by banani on 17/02/26.
//

#ifndef POLINOMS_H
#define POLINOMS_H
#include <bits/stdc++.h>
#include "ForwardList.h"
using std::vector, std::sort, std::pair, std::cout, std::cin;
#define all(x) x.begin(), x.end()
#define pint pair<int32_t, double>

vector<pint> compress(const vector<pint>& a) {
    vector<pint> res;
    int i = 0;
    while (i < a.size()) { // O(n)
        pint c = a[i];
        int j = i + 1;
        while (j < a.size() && a[j].first == a[i].first) {
            c.second += a[j].second;
            ++j;
        }
        if (std::abs(c.second) > 1e-9) res.push_back(c);
        i = j;
    }
    return res;
}


struct monon {
    int32_t ijk = 0;
    double a = 0;

    monon(int f, double s) {
        ijk = f; a = s;
    }

    friend monon operator+(const monon& a, const monon& b) {
        if (a.ijk != b.ijk) throw std::invalid_argument("vse ploho!");
        return {a.ijk, a.a + b.a};
    }

    friend bool operator==(const monon &a, const monon &b) {
        if (a.ijk == b.ijk && a.a == b.a) return true;
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
};



class polijop {
    ForwardList<monon> monomesi;
    void build_from_vec(vector<pint> a) {
        sort(all(a)); // O(n log n)
        a = compress(a); // O(n)
        for (auto el : a) monomesi.push_front(monon (el.first, el.second)); // O(n) po ubivaniu
    }
public:

    polijop(vector<pint> a) {
        build_from_vec(std::move(a));
    }

    polijop(const vector<monon>& v) {
        vector<pint> a;
        for (auto el : v) {
            a.emplace_back(el.ijk, el.a);
        }
        build_from_vec(a);
    }

    polijop(ForwardList<monon> m) {
        monomesi = std::move(m);
    }

    friend polijop operator*(polijop a, polijop b) { // O(n^2): 2 while eto n^2 + nlogn + n v konstruktore
        if (a.monomesi.empty() || b.monomesi.empty()) return polijop (vector<pint> {});

        vector<monon> monones;
        for (const auto& ma : a.monomesi) {
            for (const auto& mb : b.monomesi) {
                monones.push_back(ma * mb);
            }
        }
        return polijop(monones);
    }

    friend polijop operator-(polijop a, polijop b) {
        return (a + (-1 * b));
    }

    friend polijop operator+(polijop a, polijop b) {
        if (a.monomesi.empty() && b.monomesi.empty()) return polijop (vector<pint> {});
        if (a.monomesi.empty()) return b;
        if (b.monomesi.empty()) return a;
        ForwardList<monon> res;

        while (!a.monomesi.empty() && !b.monomesi.empty()) {
            monon a_el = a.monomesi.front();
            monon b_el = b.monomesi.front();
            if (a_el.ijk == b_el.ijk) {
                double new_a = a_el.a + b_el.a;
                if (std::abs(new_a) > 1e-9) {
                    res.push_front(monon(a_el.ijk, new_a));
                }
                a.monomesi.pop_front(); b.monomesi.pop_front();
            }
            else if (a_el.ijk < b_el.ijk) {
                res.push_front(b_el);
                b.monomesi.pop_front();
            }
            else {
                res.push_front(a_el);
                a.monomesi.pop_front();
            }

        }
        while (!a.monomesi.empty()) {
            res.push_front(a.monomesi.front());
            a.monomesi.pop_front();
        }
        while (!b.monomesi.empty()) {
            res.push_front(b.monomesi.front());
            b.monomesi.pop_front();
        }

        res.reverse();
        return polijop (res);
    }

    friend polijop operator+(polijop p, double val) {
        if (std::abs(val) < 1e-9) return p;
        return p + polijop(vector<pint>{{0, val}});
    }
    friend polijop operator+(double val, polijop p) { return p + val; }

    friend polijop operator-(polijop p, double val) { return p + (-val); }
    friend polijop operator-(double val, polijop p) {
        return polijop(vector<pint>{{0, val}}) + (p * -1.0);
    }

    friend polijop operator*(polijop p, double val) {
        if (std::abs(val) < 1e-9) return polijop(vector<pint>{});
        ForwardList<monon> res_list;
        for (const auto& m : p.monomesi) {
            res_list.push_front(monon(m.ijk, m.a * val));
        }
        res_list.reverse();
        return polijop(res_list);
    }
    friend polijop operator*(double val, polijop p) { return p * val; }

    friend polijop operator/(polijop p, double val) {
        if (std::abs(val) < 1e-9) throw std::runtime_error("Podelili na nol.....");
        return p * (1.0 / val);
    }

    friend bool operator==(polijop a, polijop b) {
        if (a.monomesi.size() != b.monomesi.size()) return false;
        while (!a.monomesi.empty()) {
            if (a.monomesi.front() != b.monomesi.front()) return false;
            a.monomesi.pop_front(); b.monomesi.pop_front();
        }
        return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const polijop& p) {
        if (p.monomesi.empty()) {
            os << "0";
            return os;
        }

        bool first = true;
        int32_t mask = (1 << 10) - 1;

        for (const auto& m : p.monomesi) {
            if (!first && m.a > 0) os << " + ";
            if (m.a < 0) os << (first ? "-" : " - ");

            double fsldf = std::abs(m.a);
            if (fsldf != 1.0 || m.ijk == 0) os << fsldf;

            int32_t i = m.ijk & mask;
            int32_t j = (m.ijk >> 10) & mask;
            int32_t k = (m.ijk >> 20) & mask;

            if (i > 0) { os << "x"; if (i > 1) os << "^" << i; }
            if (j > 0) { os << "y"; if (j > 1) os << "^" << j; }
            if (k > 0) { os << "z"; if (k > 1) os << "^" << k; }
            first = false;
        }
        os << '\n';
        return os;
    }

};
#endif //POLINOMS_H
