#pragma once

#include <cstddef>
#include <iostream>
#include <properties/property.h>
#include <utils/power_of_10_generator.h>

namespace experimental
{
    template <std::size_t N, typename T = std::uint64_t>
    struct IntegerStringProperty : public Property<char, N>
    {
        static constexpr const auto powerOf10 = PowerOf10Generator<N, T>(); // NOLINT

        [[nodiscard]] T Get() const {
            T result = 0;
            std::size_t i = 0;

            for (; i + 4 < N; i += 4)
            {
                T v0 = this->powerOf10[i] * (this->m_Ts[i] - '0');
                T v1 = this->powerOf10[i + 1] * (this->m_Ts[i + 1] - '0');
                T v2 = this->powerOf10[i + 2] * (this->m_Ts[i + 2] - '0');
                T v3 = this->powerOf10[i + 3] * (this->m_Ts[i + 3] - '0');

                result += v0 + v1 + v2 + v3;
            }

            for (; i < N; ++i)
            {
                result += this->powerOf10[i] * (this->m_Ts[i] - '0');
            }

            return result;
        }
    };

    template <std::size_t N, typename T>
    [[maybe_unused]] std::ostream& operator<<(std::ostream& iOStream, const IntegerStringProperty<N, T>& iProperty)
    {
        return iOStream << iProperty.Get();
    }
}
