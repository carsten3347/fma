#include "madd.h"

#include <iomanip>
#include <iostream>

namespace {

template <class MAdd>
#ifdef __GNUC__
// Clang will use a fused multiply-add add compile time (constexpr)
// even if not available at runtime.
// We prevent inlining to avoid confusing results.
__attribute__((noinline))
#endif
void demo(const MAdd& madd, double x, double y, double z)
{
    std::cout << "x = " << x << ", y = " << y << ", z = " << z << "\n";
    std::cout << "x * y = " << x * y << "\n";
    std::cout << "x * y + z = " << madd(x, y, z) << "\n\n";
}

template <class MAdd>
void demo()
{    
    MAdd madd {};
    
    std::cout << "Using " << madd.descr << "\n";
    for (const auto& [x, y, z] : {
             std::make_tuple(3.0, 1.0 / 3.0, -1.0),
             std::make_tuple(std::sqrt(2.0), std::sqrt(2.0), -2.0),
         })
        demo(madd, x, y, z);
}

}

int main()
{
    std::cout << std::setprecision(17);
    for (int pass = 0; pass < 2; ++pass) {
        demo<madd::TwoExprMAdd>();
        demo<madd::StdFMA>();
        demo<madd::OneExprMAdd>();
        std::hexfloat(std::cout);
    }
}
