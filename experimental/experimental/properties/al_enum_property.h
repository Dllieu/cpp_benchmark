#pragma once

#include <properties/property.h>

namespace experimental
{
    template <typename T>
    struct AlphaNumericEnumProperty : public Property<T>
    {
        [[maybe_unused]] std::ostream& Stream(std::ostream& iOStream) const {
            for (std::size_t i = 0; i < sizeof(T); ++i)
            {
                iOStream << reinterpret_cast<const char*>(std::addressof(this->m_T))[i];
            }

            return iOStream;
        }
    };

    template <typename T>
    [[maybe_unused]] std::ostream& operator<<(std::ostream& iOStream, const AlphaNumericEnumProperty<T>& iProperty)
    {
        return iProperty.Stream(iOStream);
    }
}
