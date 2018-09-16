#pragma once

#include <bits/byteswap.h>
#include <iostream>
#include <type_traits>
#include <utils/macros.h>
#include <utils/traits.h>

namespace experimental
{
    template <typename T>
    struct SwapEndiannessProperty
    {
        [[nodiscard]] force_inline T Get() const {
            if constexpr (2 == sizeof(T))
            {
                return static_cast<T>(__bswap_16(static_cast<std::uint16_t>(this->m_T)));
            }
            else if constexpr (4 == sizeof(T))
            {
                return static_cast<T>(__bswap_32(static_cast<std::uint16_t>(this->m_T)));
            }
            else if constexpr (8 == sizeof(T))
            {
                return static_cast<T>(__bswap_64(static_cast<std::uint16_t>(this->m_T)));
            }
            else
            {
                static_assert(false == std::is_same_v<T, T>, "T do not match size requirement!");
            }
        }

        T m_T;
    };

    template <typename T>
    [[maybe_unused]] std::ostream& operator<<(std::ostream& iOStream, const SwapEndiannessProperty<T>& iSwapEndiannessProperty)
    {
        if constexpr (true == std::is_enum_v<T>)
        {
            return iOStream << enum_underlying_cast(iSwapEndiannessProperty.Get());
        }
        else
        {
            return iOStream << iSwapEndiannessProperty.Get();
        }
    }
}
