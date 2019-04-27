#pragma once

#include <properties/property.h>
#include <utils/power_of_10_generator.h>

namespace experimental
{
    enum class BinaryCodedDecimalType : std::uint8_t
    {
        Packed = 0,
        Unpacked = 1,
    };

    template <std::size_t N, typename T = std::uint64_t, std::size_t NumberDecimal = 0, BinaryCodedDecimalType BCDType = BinaryCodedDecimalType::Packed>
    struct BinaryCodedDecimal : public Property<std::uint8_t, N>
    {
        using IntegerT = std::conditional_t<std::is_floating_point_v<T>, std::conditional_t<std::is_same_v<T, double>, std::uint64_t, std::uint32_t>, T>;

        [[nodiscard]] inline T Get() const
        {
            static constexpr const auto powerOf10 = PowerOf10Generator<N, IntegerT>();
            T result = 0;

            for (std::size_t i = 0; i < N; ++i)
            {
                std::uint8_t byte;

                if constexpr(1u == N)
                {
                    byte = this->m_T;
                }
                else if constexpr(1u != N)
                {
                    byte = this->m_Ts[i];
                }

                if constexpr (BinaryCodedDecimalType::Packed == BCDType)
                {
                    std::uint8_t leastSignificantNibble = byte >> 4;
                    std::uint8_t mostSignificantNibble = byte & 0x0F;

                    result += powerOf10[i] * powerOf10[i] * (leastSignificantNibble * 10 + mostSignificantNibble);
                }
                else if (BinaryCodedDecimalType::Unpacked == BCDType)
                {
                    result += powerOf10[i] * byte;
                }
            }

            result /= powerOf10[N - 1 - NumberDecimal];
            return result;
        }
    };

    template <std::size_t N, typename T, std::size_t NumberDecimal, char ValueToSkip>
    [[maybe_unused]] std::ostream& operator<<(std::ostream& iOStream, const BinaryCodedDecimal<N>& iProperty)
    {
        return iOStream << iProperty.Get();
    }
}
