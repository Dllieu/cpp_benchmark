#pragma once

#include <properties/property.h>
#include <string_view>

namespace experimental
{
    template <std::size_t N>
    struct StringProperty : public Property<char, N>
    {
        [[nodiscard]] inline std::string_view Get() const
        {
            return std::string_view(this->m_Ts.data(), N);
        }
    };

    template <std::size_t N>
    [[maybe_unused]] std::ostream& operator<<(std::ostream& iOStream, const StringProperty<N>& iProperty)
    {
        return iOStream << iProperty.Get();
    }
}
