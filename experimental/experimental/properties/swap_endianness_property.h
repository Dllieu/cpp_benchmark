#pragma once

#include <bits/byteswap.h>
#include <iostream>
#include <memory>
#include <type_traits>
#include <utils/macros.h>
#include <utils/traits.h>

namespace experimental
{
    template <typename T>
    struct SwapEndiannessProperty
    {
        [[nodiscard]] force_inline T Get() const {
            if constexpr (2 == sizeof(T)) // NOLINT
            {
                std::uint16_t value = __bswap_16(*reinterpret_cast<const std::uint16_t*>(std::addressof(this->m_T)));
                return *reinterpret_cast<T*>(std::addressof(value));
            }
            else if constexpr (4 == sizeof(T))
            {
                std::uint32_t value = __bswap_32(*reinterpret_cast<const std::uint32_t*>(std::addressof(this->m_T)));
                return *reinterpret_cast<T*>(std::addressof(value));
            }
            else if constexpr (8 == sizeof(T))
            {
                std::uint64_t value = __bswap_64(*reinterpret_cast<const std::uint64_t*>(std::addressof(this->m_T)));
                return *reinterpret_cast<T*>(std::addressof(value));
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
        if constexpr (true == std::is_enum_v<T>) // NOLINT
        {
            return iOStream << enum_underlying_cast(iSwapEndiannessProperty.Get());
        }
        else
        {
            return iOStream << iSwapEndiannessProperty.Get();
        }
    }
}
