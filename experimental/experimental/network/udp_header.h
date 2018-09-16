#pragma once

#include <messages/message.h>
#include <properties/swap_endianness_property.h>

namespace experimental
{
    // clang-format off
    DECLARE_MESSAGE_LAYOUT_PACKED(UDPHeader, 8,
        (SwapEndiannessProperty<std::uint16_t>, SourcePort)
        (SwapEndiannessProperty<std::uint16_t>, DestinationPort)
        (SwapEndiannessProperty<std::uint16_t>, Length)
        (SwapEndiannessProperty<std::uint16_t>, Checksum));
    // clang-format on
}
