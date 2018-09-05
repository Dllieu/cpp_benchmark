#include <array>
#include <cstddef>
#include <functional>
#include <gtest/gtest.h>
#include <memory>

TEST(FunctorTypeErasureTest, Lambda) // NOLINT
{
    auto emptyLambda = [] {};
    static_assert(1u == sizeof(emptyLambda));

    std::array<std::byte, 18> array{};
    auto referenceLambda = [&array] { array.fill(std::byte{}); }; // NOLINT
    static_assert(8u == sizeof(referenceLambda));

    auto constCopyLambda = [array]() { return array.empty(); }; // NOLINT
    static_assert(18u == sizeof(constCopyLambda));

    auto copyLambda = [array]() mutable { array.fill(std::byte{}); }; // NOLINT
    static_assert(18u == sizeof(copyLambda));
}

namespace
{
    template <typename R, typename... Args>
    class FunctionWrapperBaseImpl
    {
    public:
        virtual ~FunctionWrapperBaseImpl() = default;

        virtual R Invoke(Args&&... iArgs) const = 0;
        virtual std::size_t FunctorSize() const = 0;
    };

    template <typename Functor, typename R, typename... Args>
    class FunctionWrapperImpl final
        : public FunctionWrapperBaseImpl<R, Args...>
        , public Functor
    {
    public:
        using BaseT = FunctionWrapperBaseImpl<R, Args...>;

        template <typename FunctorT>
        explicit FunctionWrapperImpl(FunctorT&& iFunctor) // NOLINT
            : Functor(std::forward<FunctorT>(iFunctor))
        {
        }

        R Invoke(Args&&... iArgs) const final
        {
            return (static_cast<const Functor*>(this))->operator()(std::forward<Args>(iArgs)...);
        }

        std::size_t FunctorSize() const final
        {
            return sizeof(*this);
        }
    };

    template <typename Signature>
    struct HeapAllocationFunctionWrapper;

    template <typename R, typename... Args>
    struct HeapAllocationFunctionWrapper<R(Args...)>
    {
    public:
        HeapAllocationFunctionWrapper() = default;

        template <typename F>
        explicit HeapAllocationFunctionWrapper(F&& iFunctor) // NOLINT
        {
            this->m_Function.reset(new FunctionWrapperImpl<std::decay_t<F>, R, Args...>(std::forward<F>(iFunctor)));
        }

        template <typename F>
        HeapAllocationFunctionWrapper& operator=(F&& iFunction)
        {
            HeapAllocationFunctionWrapper tmp(std::forward<F>(iFunction));
            std::swap(*this, tmp);

            return *this;
        }

        explicit operator bool() const
        {
            return this->m_Function.operator bool();
        }

        R operator()(Args&&... iArgs) const
        {
            return this->m_Function->Invoke(std::forward<Args>(iArgs)...);
        }

        std::size_t TotalSize() const
        {
            return sizeof(*this) + (*this ? this->m_Function->FunctorSize() : 0u);
        }

    private:
        std::unique_ptr<FunctionWrapperBaseImpl<R, Args...>> m_Function;
    };
}

TEST(FunctorTypeErasureTest, HeapAllocationFunctionWrapper) // NOLINT
{
    HeapAllocationFunctionWrapper<void(void)> emptyWrapper;
    EXPECT_EQ(8u, emptyWrapper.TotalSize()); // ptr

    auto emptyLambda = [] {};
    emptyWrapper = emptyLambda;
    EXPECT_EQ(16u, emptyWrapper.TotalSize()); // ptr + vtable + 0 (EBO)

    std::int32_t n = 0;
    auto referenceLambda1 = [&n] { n = 42; };
    HeapAllocationFunctionWrapper<void(void)> referenceWrapper(referenceLambda1);
    referenceWrapper();
    EXPECT_EQ(42, n); // ptr + vtable + ptr

    std::array<std::byte, 18> array{};
    auto referenceLambda2 = [&array] { array.fill(std::byte{}); };
    referenceWrapper = referenceLambda2;
    EXPECT_EQ(24u, referenceWrapper.TotalSize()); // ptr + vtable + ptr

    auto constCopyLambda = [array]() { return array.empty(); };
    HeapAllocationFunctionWrapper<bool(void)> copyWrapper(constCopyLambda);
    EXPECT_EQ(40u, copyWrapper.TotalSize()); // ptr + vtable + 24 (18 alignas(8))
}

namespace
{
    template <std::size_t BufferSize, typename Signature>
    struct StackAllocationFunctionWrapper;

    template <std::size_t BufferSize, typename R, typename... Args>
    struct StackAllocationFunctionWrapper<BufferSize, R(Args...)>
    {
    public:
        StackAllocationFunctionWrapper()
            : m_Buffer{}
            , m_Pointer(nullptr)
        {
        }

        template <typename F>
        explicit StackAllocationFunctionWrapper(F&& iFunctor) // NOLINT
        {
            static_assert(sizeof(F) <= BufferSize, "Functor overflow!");
            this->m_Pointer = new (this->m_Buffer.data()) FunctionWrapperImpl<std::decay_t<F>, R, Args...>(std::forward<F>(iFunctor));
        }

        template <typename F>
        StackAllocationFunctionWrapper& operator=(F&& iFunction)
        {
            StackAllocationFunctionWrapper tmp(std::forward<F>(iFunction));
            std::swap(*this, tmp);

            return *this;
        }

        R operator()(Args&&... iArgs) const
        {
            return this->m_Pointer->Invoke(std::forward<Args>(iArgs)...);
        }

    private:
        std::array<std::byte, BufferSize + sizeof(FunctionWrapperBaseImpl<R, Args...>)> m_Buffer;
        FunctionWrapperBaseImpl<R, Args...>* m_Pointer;
    };
}

TEST(FunctorTypeErasureTest, StackAllocationFunctionWrapper) // NOLINT
{
    StackAllocationFunctionWrapper<16, void(void)> emptyWrapper;
    static_assert(32u == sizeof(emptyWrapper)); // 16 (static buffer) + vtable + ptr

    std::int32_t n = 0;
    auto referenceLambda = [&n] { n = 42; };
    StackAllocationFunctionWrapper<16, void(void)> referenceWrapper(referenceLambda);
    static_assert(32u == sizeof(referenceWrapper));
    referenceWrapper();
    EXPECT_EQ(42, n);

    // // static_assert will trigger
    // std::array<std::byte, 18> array{};
    // auto constCopyLambda = [array] { array.fill(std::byte{}); };
    // referenceWrapper = constCopyLambda;
}
