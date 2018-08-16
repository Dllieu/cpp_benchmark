#include <gtest/gtest.h>
#include <utils/macros.h>
#include <sstream>
#include <array>
#include <cstdint>

#define PROPERTY(PROPERTY_TYPE, PROPERTY_NAME)                                                              \
    public:                                                                                                 \
        inline const PROPERTY_TYPE& Get##PROPERTY_NAME() const { return this->m_##PROPERTY_NAME; }          \
                                                                                                            \
        template <typename T>                                                                               \
        inline void Set##PROPERTY_NAME(T&& iValue) { this->m_##PROPERTY_NAME = std::forward<T>(iValue); }   \
                                                                                                            \
    protected:                                                                                              \
        PROPERTY_TYPE m_##PROPERTY_NAME;

#define PROPERTY_ALIAS(PROPERTY_TYPE, PROPERTY_NAME, PROPERTY_ALIAS_NAME)                                       \
    public:                                                                                                     \
        inline const PROPERTY_TYPE& Get##PROPERTY_ALIAS_NAME() const { return this->m_##PROPERTY_NAME; }        \
                                                                                                                \
        template <typename T>                                                                                   \
        inline void Set##PROPERTY_ALIAS_NAME(T&& iValue) { this->m_##PROPERTY_NAME = std::forward<T>(iValue); } \
                                                                                                                \
        PROPERTY(PROPERTY_TYPE, PROPERTY_NAME)

#define GET_4TH_ARG(_1, _2, _3, _4, ...) _4
#define DECLARE_PROPERTY_WITH_ALIAS(...) \
    GET_4TH_ARG(__VA_ARGS__, PROPERTY_ALIAS, PROPERTY)

#define VA_DECLARE_PROPERTIES_IMPL(...) \
    DECLARE_PROPERTY_WITH_ALIAS(__VA_ARGS__) \
    (__VA_ARGS__)

#define VA_DECLARE_PROPERTIES_0_END
#define VA_DECLARE_PROPERTIES_1_END
#define VA_DECLARE_PROPERTIES_0(...) \
    VA_DECLARE_PROPERTIES_IMPL(__VA_ARGS__) \
    VA_DECLARE_PROPERTIES_1
#define VA_DECLARE_PROPERTIES_1(...) \
    VA_DECLARE_PROPERTIES_IMPL(__VA_ARGS__) \
    VA_DECLARE_PROPERTIES_0
#define VA_DECLARE_PROPERTIES(...) \
    VA_DECLARE_PROPERTIES_IMPL(__VA_ARGS__) \
    VA_DECLARE_PROPERTIES_0

#define VA_STREAM_FIRST_IMPL(MACRO_TO_SKIP, PROPERTY_NAME, ...) \
    #PROPERTY_NAME "=" << iMessageLayout.Get##PROPERTY_NAME()
#define VA_STREAM_IMPL(MACRO_TO_SKIP, PROPERTY_NAME, ...) \
    << " | " #PROPERTY_NAME "=" << iMessageLayout.Get##PROPERTY_NAME()

#define VA_STREAM_0_END
#define VA_STREAM_1_END
#define VA_STREAM(...) \
    VA_STREAM_FIRST_IMPL(__VA_ARGS__) \
    VA_STREAM_0
#define VA_STREAM_0(...) \
    VA_STREAM_IMPL(__VA_ARGS__) \
    VA_STREAM_1
#define VA_STREAM_1(...) \
    VA_STREAM_IMPL(__VA_ARGS__) \
    VA_STREAM_0

#define DECLARE_MESSAGE(MESSAGE_NAME, LAYOUT_SIZE, ...)                                     \
    struct MESSAGE_NAME                                                                     \
    {                                                                                       \
        VA_FOR_EACH(VA_DECLARE_PROPERTIES __VA_ARGS__);                                     \
    };                                                                                      \
                                                                                            \
    template <std::size_t ExpectedLayoutSize, std::size_t CurrentLayoutSize>                \
    static constexpr bool StaticSizeChecker()                                               \
    {                                                                                       \
        static_assert(ExpectedLayoutSize == CurrentLayoutSize, "Layout size mismatch!");    \
        return true;                                                                        \
    };                                                                                      \
    static_assert(true == StaticSizeChecker<LAYOUT_SIZE, sizeof(MESSAGE_NAME)>());          \
                                                                                            \
    std::ostream& operator<<(std::ostream& iOStream, const MESSAGE_NAME& iMessageLayout)    \
    {                                                                                       \
        iOStream << #MESSAGE_NAME " [" VA_FOR_EACH(VA_STREAM __VA_ARGS__) << "]";           \
        return iOStream;                                                                    \
    }

namespace
{
#pragma pack(push, 1)

    DECLARE_MESSAGE(Message1, 20,
        (std::uint32_t, Id, ID)
        (std::uint64_t, Price)
        (std::uint64_t, Quantity));

#pragma pack(pop)
}

TEST(MessageTest, Usage)
{
    std::array<char, sizeof(Message1)> buffer{};
    Message1* m = reinterpret_cast<Message1*>(buffer.data());

    EXPECT_EQ(0u, m->GetId());
    EXPECT_EQ(0u, m->GetPrice());
    EXPECT_EQ(0u, m->GetQuantity());

    std::stringstream ss;
    ss << *m;

    EXPECT_EQ("Message1 [Id=0 | Price=0 | Quantity=0]", ss.str());
}
