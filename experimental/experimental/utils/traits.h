#pragma once

#include <experimental/type_traits>
#include <type_traits>
#include <utility>

#define DECLARE_HAS_MEMBER(MEMBER_NAME)                                          \
    template <typename T>                                                        \
    using HasMemberImpl_##MEMBER_NAME = decltype(std::declval<T>().MEMBER_NAME); \
                                                                                 \
    template <typename T>                                                        \
    [[maybe_unused]] constexpr bool HasMember_##MEMBER_NAME = std::experimental::is_detected_v<HasMemberImpl_##MEMBER_NAME, T>

#define DECLARE_HAS_METHOD(METHOD_NAME)                                                                   \
    template <typename T, typename... Args>                                                               \
    using HasMethodImpl_##METHOD_NAME = decltype(std::declval<T>().METHOD_NAME(std::declval<Args>()...)); \
                                                                                                          \
    template <typename T, typename... Args>                                                               \
    [[maybe_unused]] constexpr bool HasMethod_##METHOD_NAME = std::experimental::is_detected_v<HasMethodImpl_##METHOD_NAME, T, Args...>

#define DECLARE_HAS_TYPE(TYPE_NAME)                        \
    template <typename T>                                  \
    using HasTypeImpl_##TYPE_NAME = typename T::TYPE_NAME; \
                                                           \
    template <typename T>                                  \
    [[maybe_unused]] constexpr bool HasType_##TYPE_NAME = std::experimental::is_detected_v<HasTypeImpl_##TYPE_NAME, T>

namespace experimental
{
    template <std::size_t Expected, std::size_t Current>
    constexpr bool EnsureEqual()
    {
        static_assert(Expected == Current, "Mismatch!");
        return true;
    }

    template <typename EnumT>
    constexpr auto enum_cast(EnumT v)
    {
        return static_cast<std::underlying_type_t<EnumT>>(v);
    }
}
