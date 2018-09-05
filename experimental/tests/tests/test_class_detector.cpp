#include <cstddef>
#include <gtest/gtest.h>
#include <type_traits>
#include <utility>
#include <utils/macros.h>
#include <utils/traits.h>
#include <vector>

namespace
{
    DECLARE_HAS_TYPE(value_type);
}

TEST(TraitsTest, HasType) // NOLINT
{
    static_assert(false == HasType_value_type<int>);
    static_assert(true == HasType_value_type<std::vector<int>>);
}

namespace
{
    struct Foo
    {
    public:
        int memberA;

    private:
        int memberB;
    };

    DECLARE_HAS_MEMBER(memberA);
    DECLARE_HAS_MEMBER(memberB);
}

TEST(TraitsTest, HasMember) // NOLINT
{
    static_assert(true == HasMember_memberA<Foo>);
    static_assert(false == HasMember_memberB<Foo>);
}

#define VERY_OLD_DECLARE_HAS_METHOD(METHOD_NAME)                                          \
    template <typename T>                                                                 \
    class VeryOldHasMethod_##METHOD_NAME                                                  \
    {                                                                                     \
        using succeed = std::byte;                                                        \
        using fail = std::int32_t;                                                        \
        static_assert(sizeof(succeed) != sizeof(fail));                                   \
                                                                                          \
        template <typename C>                                                             \
        static succeed test(decltype(&C::METHOD_NAME));                                   \
                                                                                          \
        template <typename C>                                                             \
        static fail test(...);                                                            \
                                                                                          \
    public:                                                                               \
        static constexpr bool value = sizeof(test<T>(0)) == sizeof(succeed); /* NOLINT */ \
    }

#define OLD_DECLARE_HAS_METHOD(METHOD_NAME)                                                                       \
    template <typename T, typename = std::void_t<>>                                                               \
    struct OldHasMethod_##METHOD_NAME : std::false_type                                                           \
    {                                                                                                             \
    };                                                                                                            \
                                                                                                                  \
    template <typename T>                                                                                         \
    struct OldHasMethod_##METHOD_NAME<T, std::void_t<decltype(std::declval<T>().METHOD_NAME())>> : std::true_type \
    {                                                                                                             \
    }

namespace
{
    struct Bar
    {
        int bar(int a) const;
        int bar(const std::string& a) const;
    };

    VERY_OLD_DECLARE_HAS_METHOD(bar);
    VERY_OLD_DECLARE_HAS_METHOD(size);

    OLD_DECLARE_HAS_METHOD(bar);
    OLD_DECLARE_HAS_METHOD(size);
}

TEST(TraitsTest, HasMethodNoArgumentCheck) // NOLINT
{
    static_assert(true == VeryOldHasMethod_size<std::vector<int>>::value);
    static_assert(false == VeryOldHasMethod_size<Bar>::value);
    static_assert(false == VeryOldHasMethod_bar<Bar>::value);

    static_assert(true == OldHasMethod_size<std::vector<int>>::value);
    static_assert(false == OldHasMethod_size<Bar>::value);
    static_assert(false == OldHasMethod_bar<Bar>::value);
}

namespace
{
    // std::experimental::is_detected
    template <typename AlwaysVoid, template <typename...> typename Op, typename... Args>
    struct ExampleHasBarImpl : std::false_type
    {
    };

    template <template <typename...> typename Op, typename... Args>
    struct ExampleHasBarImpl<std::void_t<Op<Args...>>, Op, Args...> : std::true_type
    {
    };
    // ! std::experimental::is_detected

    template <typename T, typename... Args>
    using ExampleHasBarOp = decltype(std::declval<T>().bar(std::declval<Args>()...));

    template <typename T, typename... Args>
    constexpr bool ExampleHasBar = ExampleHasBarImpl<void, ExampleHasBarOp, T, Args...>();
}

namespace
{
    DECLARE_HAS_METHOD(bar);
}

TEST(TraitsTest, HasMethodArgumentCheck) // NOLINT
{
    static_assert(false == ExampleHasBar<Bar>);
    static_assert(true == ExampleHasBar<Bar, int>);
    static_assert(true == ExampleHasBar<Bar, std::string>);

    static_assert(false == HasMethod_bar<Bar>);
    static_assert(true == HasMethod_bar<Bar, int>);
    static_assert(true == HasMethod_bar<Bar, std::string>);
}
