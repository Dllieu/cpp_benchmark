#pragma once

#include <properties/property.h>
#include <utils/power_of_10_generator.h>
#include <cmath>

namespace experimental
{
    enum class BinaryCodedDecimalType : std::uint8_t
    {
        Packed = 0,
        Unpacked = 1,
    };

    template <std::size_t N, BinaryCodedDecimalType BCDType = BinaryCodedDecimalType::Packed, typename T = std::uint64_t>
    struct BinaryCodedDecimalProperty : public Property<std::uint8_t, N>
    {
        static constexpr const auto powerOf10 = PowerOf10Generator<N, T>();

        [[nodiscard]] inline T Get() const
        {
            T result = 0;
            std::size_t i = 0;

            for (; i + 4 < N; i += 4)
            {
                T v0 = this->ComputeByte(i);
                T v1 = this->ComputeByte(i + 1);
                T v2 = this->ComputeByte(i + 2);
                T v3 = this->ComputeByte(i + 3);

                result += v0 + v1 + v2 + v3;
            }

            for (; i < N; ++i)
            {
                result += this->ComputeByte(i);
            }

            return result;
        }

    private:
        inline T ComputeByte(std::size_t iBytePosition) const
        {
            std::uint8_t byte;

            if constexpr(1u == N)
            {
                byte = this->m_T;
            }
            else if constexpr(1u != N)
            {
                byte = this->m_Ts[iBytePosition];
            }

            if constexpr (BinaryCodedDecimalType::Packed == BCDType)
            {
                std::uint8_t leastSignificantNibble = byte >> 4;
                std::uint8_t mostSignificantNibble = byte & 0x0F;

                return this->powerOf10[iBytePosition]
                    * this->powerOf10[iBytePosition]
                    * (leastSignificantNibble * 10 + mostSignificantNibble);
            }
            else if constexpr (BinaryCodedDecimalType::Unpacked == BCDType)
            {
                return this->powerOf10[iBytePosition] * byte;
            }
        }
    };

    template <std::size_t N, BinaryCodedDecimalType BCDType, typename T>
    [[maybe_unused]] std::ostream& operator<<(std::ostream& iOStream, const BinaryCodedDecimalProperty<N, BCDType, T>& iProperty)
    {
        return iOStream << iProperty.Get();
    }
}
