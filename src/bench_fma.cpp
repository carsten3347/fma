#include <Eigen/Core>

#include <benchmark/benchmark.h>

#include <cmath>
#include <cstdint>

#pragma STDC FP_CONTRACT DEFAULT

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

struct ForcedFMA {
    double operator()(double x, double y, double a) { return std::fma(x, y, a); }
};

struct OneExpr {
    double operator()(double x, double y, double a) { return x * y + a; }
};

struct TwoExprs {
    double operator()(double x, double y, double a)
    {
        const auto p = x * y;
        return p + a;
    }
};

struct alignas(16) Vec {
    double x_, y_;

    double& x()
    {
	return x_;
    }
    const double& x() const
    {
	return x_;
    }
    double& y()
    {
	return y_;
    }
    const double& y() const
    {
	return y_;
    }
};

struct VecOneExpr : public Vec {
    double dot(const Vec& o) const
    {
        return x_ * o.x_ + y_ * o.y_;
    }
};

struct VecMultExpr : public Vec {
    double dot(const Vec& o) const
    {
        const auto px = x_ * o.x_;
        const auto py = y_ * o.y_;
        return px + py;
    }
};

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

template <class MADD, int N>
double
evalN(const double* coeffs, double x)
{
    MADD madd {};

    double v = coeffs[N];
    for (auto i = N; i > 0; --i)
        v = madd(x, v, coeffs[i - 1]);
    return v;
};

template <class MADD, int N>
void BM_Horner_N(benchmark::State& state)
{
    const auto c = expCoeffs(N);
    double x = -0.1;
    for (auto _ : state) {
        x += 1e-10;
        benchmark::DoNotOptimize(evalN<MADD, N>(c.data(), x));
        // benchmark::DoNotOptimize(eval<MADD>(c, x));
    }
}

template <class MADD, int M>
void BM_Horner_UpTo(benchmark::State& state)
{
    const int N = state.range(0);
    if constexpr (M < 1) {
        throw std::runtime_error("N < 1");
    } else if (N > M) {
        throw std::runtime_error("N too large");
    } else if (N == M) {
        BM_Horner_N<MADD, M>(state);
    } else {
        BM_Horner_UpTo<MADD, M - 1>(state);
    }
}

template <class MADD>
void BM_Horner(benchmark::State& state)
{
    BM_Horner_UpTo<MADD, 20>(state);
}

template <class V>
void BM_dot(benchmark::State& state)
{
    static constexpr std::size_t N = 256;

    std::vector<std::pair<V, V>> data;
    {
        V v1 { -0.3, 100.1 };
        V v2 { 1e6, 1e6 / 3 };

        while (data.size() < N) {
            v1.x() = std::nextafter(v1.x(), 0.0);
            v2.x() = std::nextafter(v2.x(), 0.0);
            v1.y() = std::nextafter(v1.y(), 0.0);
            v2.y() = std::nextafter(v2.y(), 0.0);
            data.emplace_back(v1, v2);
        }
    }

    for (auto _ : state) {
        for (std::size_t i = 0; i < N; ++i) {
            const auto& [v1, v2] = data[i];
            benchmark::DoNotOptimize(v1.dot(v2));
        }
    }
}

} // namespace

BENCHMARK(BM_Horner<ForcedFMA>)
    ->Arg(1)
    ->Arg(2)
    ->Arg(3)
    ->Arg(4)
    ->Arg(8)
    ->Arg(16);
BENCHMARK(BM_Horner<OneExpr>)->Arg(1)->Arg(2)->Arg(3)->Arg(4)->Arg(8)->Arg(16);
BENCHMARK(BM_Horner<TwoExprs>)->Arg(1)->Arg(2)->Arg(3)->Arg(4)->Arg(8)->Arg(16);

BENCHMARK(BM_dot<VecOneExpr>);
BENCHMARK(BM_dot<VecMultExpr>);
BENCHMARK(BM_dot<Eigen::Vector2d>);

BENCHMARK_MAIN();
