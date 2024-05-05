#include "madd.h"

#include <benchmark/benchmark.h>

#include <cstdint>
#include <vector>

// #pragma STDC FP_CONTRACT DEFAULT

namespace {

std::vector<double>
expCoeffs(std::size_t N)
{
    std::vector<double> coeffs;

    double f = 1.0;
    coeffs.push_back(1.0);
    for (int i = 1; i <= N; ++i) {
        f *= i;
        coeffs.push_back(1.0 / f);
    }
    return coeffs;
}

template <class MADD>
double
eval(const std::vector<double>& coeffs, double x)
{
    MADD madd {};

    const int N = coeffs.size() - 1;
    double v = coeffs[N];
    for (auto i = N; i > 0; --i)
        v = madd(x, v, coeffs[i - 1]);
    return v;
};

template <class MADD>
void BM_Horner(benchmark::State& state)
{
    const int N = state.range(0);
    const auto c = expCoeffs(N);
    double x = -0.1;
    for (auto _ : state) {
        x += 1e-10;
        benchmark::DoNotOptimize(eval<MADD>(c, x));
    }
}

} // namespace

BENCHMARK(BM_Horner<madd::StdFMA>)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(8)
    ->Arg(16);
BENCHMARK(BM_Horner<madd::OneExprMAdd>)->Arg(1)->Arg(2)->Arg(3)->Arg(4)->Arg(8)->Arg(16);
BENCHMARK(BM_Horner<madd::TwoExprMAdd>)->Arg(1)->Arg(2)->Arg(3)->Arg(4)->Arg(8)->Arg(16);

BENCHMARK_MAIN();
