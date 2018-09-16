#pragma once

#include <cstddef>
#include <utils/macros.h>

namespace experimental
{
    constexpr force_inline bool GetBit(std::uint8_t iByte, std::size_t iBitPosition)
    {
        return 0 != (iByte & (1 << iBitPosition));
    }
}
