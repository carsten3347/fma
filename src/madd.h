#pragma once

#include <cmath>

namespace madd {

struct StdFMA {
    double operator()(double x, double y, double z) const { return std::fma(x, y, z); }

    static constexpr auto descr = "std::fma()";
};

struct OneExprMAdd {
    double operator()(double x, double y, double z) const { return x * y + z; }

    static constexpr auto descr = "x * y + z";
};

struct TwoExprMAdd {
    double operator()(double x, double y, double z) const
    {
        const auto p = x * y;
        return p + z;
    }

    static constexpr auto descr = "p = x * y; p + z";
};

}
