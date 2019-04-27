#pragma once

#include <properties/binary_coded_decimal_property.h>

namespace experimental
{
    template <std::size_t N, std::size_t NumberDecimal = 0, BinaryCodedDecimalType BCDType = BinaryCodedDecimalType::Packed, typename T = double>
    struct BinaryCodedFloatingPointProperty : public BinaryCodedDecimalProperty<N, BCDType, std::conditional_t<std::is_same_v<T, double>, std::uint64_t, std::uint32_t>>
    {
        [[nodiscard]] inline T Get() const
        {
            T result = BinaryCodedDecimalProperty<N, BCDType, std::conditional_t<std::is_same_v<T, double>, std::uint64_t, std::uint32_t>>::Get();

            if constexpr (BinaryCodedDecimalType::Packed == BCDType)
            {
                static constexpr const auto powerOf10Adjusted = PowerOf10Generator<N * 2, T>();
                result /= powerOf10Adjusted[N * 2 - 1 - NumberDecimal];
            }
            if constexpr (BinaryCodedDecimalType::Unpacked == BCDType)
            {
                result /= this->powerOf10[N - 1 - NumberDecimal];
            }

            return result;
        }
    };

    template <std::size_t N, std::size_t NumberDecimal, BinaryCodedDecimalType BCDType, typename T>
    [[maybe_unused]] std::ostream& operator<<(std::ostream& iOStream, const BinaryCodedFloatingPointProperty<N, NumberDecimal, BCDType, T>& iProperty)
    {
        return iOStream << iProperty.Get();
    }
}
