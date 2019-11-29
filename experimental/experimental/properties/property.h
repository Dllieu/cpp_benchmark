#pragma once

#include <array>
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <utils/macros.h>
#include <utils/traits.h>

namespace experimental
{
    // clang-format off
    struct property_tag_array {};
    struct property_tag_enum {};
    struct property_tag_single {};

    template <typename T, size_t N>
    using property_tag = std::conditional_t<N == 1, std::conditional_t<std::is_enum_v<T>, property_tag_enum,
                                                                                          property_tag_single>,
                                                    property_tag_array>;
    // clang-format on

    template <typename T, std::size_t N = 1, typename tag = property_tag<T, N>>
    struct Property;

    template <typename T, std::size_t N>
    struct Property<T, N, property_tag_array>
    {
        [[nodiscard]] force_inline const std::array<T, N>& Get() const
        {
            return this->m_Ts;
        }

        [[maybe_unused]] no_inline std::ostream& Stream(std::ostream& iOStream) const
        {
            iOStream << "[";

            for (std::size_t i = 0; i < N; ++i)
            {
                iOStream << this->m_Ts[i];

                if (N - 1 != i)
                {
                    iOStream << ";";
                }
            }

            iOStream << "]";

            return iOStream;
        }

        std::array<T, N> m_Ts;
    };

    template <typename T, std::size_t N>
    struct Property<T, N, property_tag_enum>
    {
        [[nodiscard]] force_inline operator T() const // NOLINT
        {
            return this->m_T;
        }

        [[nodiscard]] force_inline T Get() const
        {
            return this->m_T;
        }

        [[maybe_unused]] no_inline std::ostream& Stream(std::ostream& iOStream) const
        {
            return iOStream << experimental::enum_underlying_cast(this->m_T);
        }

        T m_T;
    };

    template <typename T, std::size_t N>
    struct Property<T, N, property_tag_single>
    {
        [[nodiscard]] force_inline operator const T&() const // NOLINT
        {
            return this->m_T;
        }
        [[nodiscard]] force_inline const T& Get() const
        {
            return this->m_T;
        }

        [[maybe_unused]] no_inline std::ostream& Stream(std::ostream& iOStream) const
        {
            return iOStream << this->m_T;
        }

        T m_T;
    };

    template <typename T, std::size_t N>
    [[maybe_unused]] std::ostream& operator<<(std::ostream& iOStream, const Property<T, N>& iProperty)
    {
        return iProperty.Stream(iOStream);
    }
}
