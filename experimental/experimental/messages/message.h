#pragma once

#include <cstddef>
#include <cstdint>
#include <sstream>
#include <utils/macros.h>
#include <utils/traits.h>

#define PROPERTY(PROPERTY_TYPE, PROPERTY_NAME)             \
  public:                                                  \
    inline const PROPERTY_TYPE& Get##PROPERTY_NAME() const \
    {                                                      \
        return this->m_##PROPERTY_NAME;                    \
    }                                                      \
                                                           \
    template <typename T>                                  \
    inline void Set##PROPERTY_NAME(T&& iValue)             \
    {                                                      \
        this->m_##PROPERTY_NAME = std::forward<T>(iValue); \
    }                                                      \
                                                           \
  protected:                                               \
    PROPERTY_TYPE m_##PROPERTY_NAME;

#define PROPERTY_ALIAS(PROPERTY_TYPE, PROPERTY_NAME, PROPERTY_ALIAS_NAME) \
  public:                                                                 \
    inline const PROPERTY_TYPE& Get##PROPERTY_ALIAS_NAME() const          \
    {                                                                     \
        return this->m_##PROPERTY_NAME;                                   \
    }                                                                     \
                                                                          \
    template <typename T>                                                 \
    inline void Set##PROPERTY_ALIAS_NAME(T&& iValue)                      \
    {                                                                     \
        this->m_##PROPERTY_NAME = std::forward<T>(iValue);                \
    }                                                                     \
                                                                          \
    PROPERTY(PROPERTY_TYPE, PROPERTY_NAME)

#define GET_4TH_ARG(_1, _2, _3, _4, ...) _4
#define DECLARE_PROPERTY_WITH_ALIAS(...) GET_4TH_ARG(__VA_ARGS__, PROPERTY_ALIAS, PROPERTY, )

#define VA_DECLARE_PROPERTIES_IMPL(...)      \
    DECLARE_PROPERTY_WITH_ALIAS(__VA_ARGS__) \
    (__VA_ARGS__)

#define VA_DECLARE_PROPERTIES_0_END
#define VA_DECLARE_PROPERTIES_1_END
#define VA_DECLARE_PROPERTIES_0(...)        \
    VA_DECLARE_PROPERTIES_IMPL(__VA_ARGS__) \
    VA_DECLARE_PROPERTIES_1
#define VA_DECLARE_PROPERTIES_1(...)        \
    VA_DECLARE_PROPERTIES_IMPL(__VA_ARGS__) \
    VA_DECLARE_PROPERTIES_0
#define VA_DECLARE_PROPERTIES(...)          \
    VA_DECLARE_PROPERTIES_IMPL(__VA_ARGS__) \
    VA_DECLARE_PROPERTIES_0

#define VA_STREAM_FIRST_IMPL(MACRO_TO_SKIP, PROPERTY_NAME, ...) #PROPERTY_NAME "=" << iMessageLayout.Get##PROPERTY_NAME()
#define VA_STREAM_IMPL(MACRO_TO_SKIP, PROPERTY_NAME, ...) << " | " #PROPERTY_NAME "=" << iMessageLayout.Get##PROPERTY_NAME()

#define VA_STREAM_0_END
#define VA_STREAM_1_END
#define VA_STREAM(...)                  \
    VA_STREAM_FIRST_IMPL(__VA_ARGS__, ) \
    VA_STREAM_0
#define VA_STREAM_0(...)          \
    VA_STREAM_IMPL(__VA_ARGS__, ) \
    VA_STREAM_1
#define VA_STREAM_1(...)          \
    VA_STREAM_IMPL(__VA_ARGS__, ) \
    VA_STREAM_0

#define DECLARE_LAYOUT_IMPL(LAYOUT_NAME, ...)          \
    struct LAYOUT_NAME##Layout                         \
    {                                                  \
        VA_FOR_EACH(VA_DECLARE_PROPERTIES __VA_ARGS__) \
    };

#define DEFINE_LAYOUT_STREAM(LAYOUT_NAME, LAYOUT_SIZE, ...)                                                      \
    [[maybe_unused]] std::ostream& operator<<(std::ostream& iOStream, const LAYOUT_NAME##Layout& iMessageLayout) \
    {                                                                                                            \
        iOStream << #LAYOUT_NAME " [" VA_FOR_EACH(VA_STREAM __VA_ARGS__) << "]";                                 \
        return iOStream;                                                                                         \
    }                                                                                                            \
                                                                                                                 \
    static_assert(true == experimental::EnsureEqual<LAYOUT_SIZE, sizeof(LAYOUT_NAME##Layout)>())

#define DECLARE_LAYOUT_PACKED(LAYOUT_NAME, LAYOUT_SIZE, ...) \
    PRAGMA_PACK_PUSH(1)                                      \
    DECLARE_LAYOUT_IMPL(LAYOUT_NAME, __VA_ARGS__)            \
    PRAGMA_PACK_POP()                                        \
    DEFINE_LAYOUT_STREAM(LAYOUT_NAME, LAYOUT_SIZE, __VA_ARGS__)

#define DECLARE_LAYOUT_CUSTOM_PACK(LAYOUT_NAME, LAYOUT_SIZE, PACK_REQUIREMENT, ...) \
    PRAGMA_PACK_PUSH(PACK_REQUIREMENT)                                              \
    DECLARE_LAYOUT_IMPL(LAYOUT_NAME, __VA_ARGS__)                                   \
    PRAGMA_PACK_POP()                                                               \
    DEFINE_LAYOUT_STREAM(LAYOUT_NAME, LAYOUT_SIZE, __VA_ARGS__)

#define DECLARE_LAYOUT_DEFAULT_ALIGNMENT(LAYOUT_NAME, LAYOUT_SIZE, ...) \
    DECLARE_LAYOUT_IMPL(LAYOUT_NAME, __VA_ARGS__)                       \
    DEFINE_LAYOUT_STREAM(LAYOUT_NAME, LAYOUT_SIZE, __VA_ARGS__)

namespace experimental
{
    template <auto MessageId, typename LayoutT>
    struct Message : public LayoutT
    {
        static constexpr auto GetExpectedMessageId()
        {
            return MessageId;
        }
    };
}

#define DECLARE_DEFAULT_MESSAGE(MESSAGE_NAME, MESSAGE_ID, DEFAULT_MESSAGE)                                                        \
    template <typename MarketTraits>                                                                                              \
    struct MESSAGE_NAME                                                                                                           \
        : public experimental::Message<MESSAGE_ID, MESSAGE_NAME##Layout>                                                          \
        , public DEFAULT_MESSAGE<MarketTraits>                                                                                    \
    {                                                                                                                             \
        static constexpr bool EnsureLayoutAndMessageMatch()                                                                       \
        {                                                                                                                         \
            static_assert(true == experimental::EnsureEqual<sizeof(MESSAGE_NAME##Layout), sizeof(MESSAGE_NAME<MarketTraits>)>()); \
            return true;                                                                                                          \
        }                                                                                                                         \
    }

#define DECLARE_MESSAGE(MESSAGE_NAME, MESSAGE_ID)                                                                                 \
    template <typename MarketTraits>                                                                                              \
    struct MESSAGE_NAME : public experimental::Message<MESSAGE_ID, MESSAGE_NAME##Layout>                                          \
    {                                                                                                                             \
        static constexpr bool EnsureLayoutAndMessageMatch()                                                                       \
        {                                                                                                                         \
            static_assert(true == experimental::EnsureEqual<sizeof(MESSAGE_NAME##Layout), sizeof(MESSAGE_NAME<MarketTraits>)>()); \
            return true;                                                                                                          \
        }                                                                                                                         \
                                                                                                                                  \
        force_inline void Process();                                                                                              \
    }
