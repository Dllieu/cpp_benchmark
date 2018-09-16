#pragma once

#include <cstddef>
#include <messages/message.h>
#include <properties/property.h>
#include <properties/swap_endianness_property.h>
#include <string>

namespace experimental
{
    enum class EthernetType : std::uint16_t
    {
        IPv4 = 0x0800,
        IPv6 = 0x86DD,
    };

    // clang-format off
    // Preamble is filtered away, we imply we do not have VLan tag
    DECLARE_MESSAGE_LAYOUT_PACKED(EthernetFrame, 14,
        ((Property<std::uint8_t, 6>), DestinationAddress)
        ((Property<std::uint8_t, 6>), SourceAddress)
        (SwapEndiannessProperty<EthernetType>, EthernetType, Length));
    // clang-format on

    struct EthernetFrame : public EthernetFrameLayout
    {
        template <typename T>
        const T* GetPayload() const
        {
            return reinterpret_cast<const T*>(reinterpret_cast<const std::byte*>(this) + sizeof(EthernetFrameLayout));
        }

        [[nodiscard]] std::string GetDestinationMACAddress() const;
        [[nodiscard]] std::string GetSourceMACAddress() const;
    };

    static_assert(sizeof(EthernetFrameLayout) == sizeof(EthernetFrame));
}
