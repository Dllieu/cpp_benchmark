#pragma once

#include <properties/property.h>
#include <string_view>

namespace experimental
{
    enum class StringPropertyType : std::uint8_t
    {
        None = 0,
        LeftJustified = 1,
        RightJustified = 2,
    };

    template <std::size_t N, StringPropertyType SPType = StringPropertyType::None, char ValueToSkip = '\0'>
    struct StringProperty : public Property<char, N>
    {
        [[nodiscard]] inline std::string_view Get() const
        {
            std::string_view result(this->m_Ts.data(), N);

            if constexpr (StringPropertyType::LeftJustified == SPType)
            {
                std::size_t endIndex = result.find_last_not_of(ValueToSkip);

                if (likely(std::string_view::npos != endIndex))
                {
                    result.remove_suffix(N - 1 - endIndex);
                }
                else
                {
                    result = std::string_view();
                }
            }
            else if constexpr (StringPropertyType::RightJustified == SPType)
            {
                std::size_t startIndex = result.find_first_not_of(ValueToSkip);

                if (likely(std::string_view::npos != startIndex))
                {
                    result.remove_prefix(startIndex);
                }
                else
                {
                    result = std::string_view();
                }
            }

            return result;
        }
    };

    template <std::size_t N, StringPropertyType SPT, char ValueToSkip>
    [[maybe_unused]] std::ostream& operator<<(std::ostream& iOStream, const StringProperty<N>& iProperty)
    {
        return iOStream << iProperty.Get();
    }
}
