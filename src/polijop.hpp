#ifndef POLIJOP_HPP
#define POLIJOP_HPP

#include "ForwardList.h"
#include "monon.hpp"
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>

using std::vector, std::pair;
using pint = pair<int32_t, double>;

inline vector<pint> compress(const vector<pint>& a) {
    vector<pint> res;
    int i = 0;
    while (i < a.size()) {
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

class polijop {
    ForwardList<monon> monomesi;

    void build_from_vec(vector<pint> a) {
        std::sort(a.begin(), a.end());
        a = compress(a);
        for (auto el : a) monomesi.push_front(monon(el.first, el.second));
    }

public:
    polijop() = default;

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

    friend polijop operator*(polijop a, polijop b) {
        if (a.monomesi.empty() || b.monomesi.empty()) return polijop(vector<pint>{});
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
        if (a.monomesi.empty() && b.monomesi.empty()) return polijop(vector<pint>{});
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
        return polijop(res);
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

    friend bool operator!=(polijop a, polijop b) {
        return !(a == b);
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

    polijop differentiate(int var) const {
        polijop res;
        for (const auto& m : monomesi) {
            monon diff = m.differentiate(var);
            if (!diff.isZero()) res.monomesi.push_front(diff);
        }
        res.monomesi.reverse();
        return res;
    }

    polijop integrate(int var) const {
        polijop res;
        for (const auto& m : monomesi) {
            monon inte = m.integrate(var);
            if (!inte.isZero()) res.monomesi.push_front(inte);
        }
        res.monomesi.reverse();
        return res;
    }

    double evaluate(double x, double y, double z) const {
        double sum = 0.0;
        for (const auto& m : monomesi)
            sum += m.evaluate(x, y, z);
        return sum;
    }

    void addMonom(const monon& m) {
        if (!m.isZero()) {
            monomesi.push_front(m);
            vector<pint> temp;
            for (const auto& mon : monomesi)
                temp.emplace_back(mon.ijk, mon.a);
            monomesi.clear();
            build_from_vec(temp);
        }
    }

    void SaveToFile(std::ostream& os) const {
        os << monomesi.size() << '\n';
        for (const auto& m : monomesi) {
            os << m.ijk << ' ' << m.a << '\n';
        }
    }

    void LoadFromFile(std::istream& is) {
        monomesi.clear();

        size_t count;
        is >> count;
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        std::vector<pint> temp;
        for (size_t i = 0; i < count; ++i) {
            int32_t ijk;
            double a;
            is >> ijk >> a;
            is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            temp.emplace_back(ijk, a);
        }
        build_from_vec(temp);
    }

    static polijop FromFile(std::istream& is) {
        polijop p;
        p.LoadFromFile(is);
        return p;
    }

    bool isZero() const { return monomesi.empty(); }
    size_t getMonomCount() const { return monomesi.size(); }
    const ForwardList<monon>& getMonoms() const { return monomesi; }
};

#endif