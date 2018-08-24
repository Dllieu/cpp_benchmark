#pragma once

#include <type_traits>

namespace experimental
{
    template <std::size_t Expected, std::size_t Current>
    constexpr bool EnsureEqual()
    {
        static_assert(Expected == Current, "Mismatch!");
        return true;
    }

    template <typename EnumT>
    constexpr auto enum_cast(EnumT v)
    {
        return static_cast<std::underlying_type_t<EnumT>>(v);
    }
}
