#include <algorithm>
#include <benchmark/benchmark.h>
#include <cstddef>
#include <memory>
#include <random>
#include <utils/macros.h>

namespace
{
    struct Base
    {
        virtual ~Base() noexcept = default;
        virtual std::size_t ReturnStuff() const
        {
            return 1;
        }
    };

    struct Derived1 final : public Base
    {
        ~Derived1() noexcept final = default;
        std::size_t ReturnStuff() const final
        {
            return 2;
        }
    };

    struct Derived2 final : public Base
    {
        ~Derived2() noexcept final = default;
        std::size_t ReturnStuff() const final
        {
            return 3;
        }
    };

    void BranchPrediction_RunBenchmark(benchmark::State& iState, const std::vector<Base*>& iBases)
    {
        std::size_t n = 0;
        for ([[maybe_unused]] auto handler : iState)
        {
            for (auto& element : iBases)
            {
                benchmark::DoNotOptimize(n += element->ReturnStuff());
            }
        }
    }

    void BranchPrediction_ShuffledVTableBenchmark(benchmark::State& iState)
    {
        auto baseElement = std::make_unique<Base>();
        auto derived1Element = std::make_unique<Derived1>();
        auto derived2Element = std::make_unique<Derived2>();
        std::vector<Base*> v;
        v.reserve(3 * iState.range(0));

        std::fill_n(std::back_inserter(v), iState.range(0), baseElement.get());
        std::fill_n(std::back_inserter(v), iState.range(0), derived1Element.get());
        std::fill_n(std::back_inserter(v), iState.range(0), derived2Element.get());

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(std::begin(v), std::end(v), g);

        BranchPrediction_RunBenchmark(iState, v);
    }

    void BranchPrediction_SortedVTableBenchmark(benchmark::State& iState)
    {
        auto baseElement = std::make_unique<Base>();
        auto derived1Element = std::make_unique<Derived1>();
        auto derived2Element = std::make_unique<Derived2>();
        std::vector<Base*> v;
        v.reserve(3 * iState.range(0));

        std::fill_n(std::back_inserter(v), iState.range(0), baseElement.get());
        std::fill_n(std::back_inserter(v), iState.range(0), derived1Element.get());
        std::fill_n(std::back_inserter(v), iState.range(0), derived2Element.get());

        BranchPrediction_RunBenchmark(iState, v);
    }

    void BranchPrediction_Arguments(benchmark::internal::Benchmark* iBenchmark)
    {
        iBenchmark->RangeMultiplier(10)->Range(1, 10'000);
    }
}

BENCHMARK(BranchPrediction_ShuffledVTableBenchmark)->Apply(BranchPrediction_Arguments); // NOLINT
BENCHMARK(BranchPrediction_SortedVTableBenchmark)->Apply(BranchPrediction_Arguments);   // NOLINT
