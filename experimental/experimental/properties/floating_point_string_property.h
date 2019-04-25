#pragma once

#include <properties/integer_string_property.h>

namespace experimental
{
    template <std::size_t N, std::size_t NumberDecimal, typename T = double, typename IntegerT = std::conditional_t<std::is_same_v<T, double>, std::uint64_t, std::uint32_t>>
    struct FloatingPointStringProperty : public IntegerStringProperty<N, IntegerT>
    {
        [[nodiscard]] T Get() const { return static_cast<T>(IntegerStringProperty<N, IntegerT>::Get()) / this->powerOf10[N - 1 - NumberDecimal]; }
    };

    template <std::size_t N, std::size_t NumberDecimal, typename T>
    [[maybe_unused]] std::ostream& operator<<(std::ostream& iOStream, const FloatingPointStringProperty<N, NumberDecimal, T>& iProperty)
    {
        return iOStream << iProperty.Get();
    }
}
