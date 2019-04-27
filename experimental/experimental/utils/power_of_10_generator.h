#pragma once

#include <cstddef>
#include <limits>

namespace experimental
{
    template <typename T>
    constexpr T PowerOf10(size_t iN)
    {
        T result = 1;

        while (iN--)
        {
            result *= 10;
        }

        return result;
    }

    template <std::size_t N, typename T>
    struct PowerOf10Generator
    {
        static_assert(N <= std::numeric_limits<T>::digits10);

        constexpr PowerOf10Generator()
            : m_Array{}
        {
            for (std::size_t i = 0; i < N; ++i)
            {
                m_Array[N - 1 - i] = PowerOf10<T>(i);
            }
        }

        constexpr T operator[](std::size_t i) const
        {
            return this->m_Array[i];
        }

        T m_Array[N];
    };
}
