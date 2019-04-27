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
                std::size_t endPosition = result.find_last_not_of(ValueToSkip);

                if (likely(std::string_view::npos != endPosition))
                {
                    result.remove_suffix(N - 1 - endPosition);
                }
                else
                {
                    result = std::string_view();
                }
            }
            else if constexpr (StringPropertyType::RightJustified == SPType)
            {
                std::size_t startPosition = result.find_first_not_of(ValueToSkip);

                if (likely(std::string_view::npos != startPosition))
                {
                    result.remove_prefix(startPosition);
                }
                else
                {
                    result = std::string_view();
                }
            }

            return result;
        }
    };

    template <std::size_t N, StringPropertyType SPType, char ValueToSkip>
    [[maybe_unused]] std::ostream& operator<<(std::ostream& iOStream, const StringProperty<N, SPType, ValueToSkip>& iProperty)
    {
        return iOStream << iProperty.Get();
    }
}
