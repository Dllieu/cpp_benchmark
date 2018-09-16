#pragma once

#include <cstddef>
#include <utils/macros.h>

namespace experimental
{
    [[maybe_unused]] no_inline void HexDump(const std::byte* iBuffer, std::size_t iLength);
}
