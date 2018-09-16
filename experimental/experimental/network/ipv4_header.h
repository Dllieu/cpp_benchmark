#pragma once

#include <cstddef>
#include <iostream>
#include <messages/message.h>
#include <properties/byte_filler_property.h>
#include <properties/property.h>
#include <properties/swap_endianness_property.h>
#include <string>
#include <utils/macros.h>

namespace experimental
{
    struct VersionWithInternetHeaderLength
    {
        force_inline std::uint8_t GetVersion() const
        {
            return static_cast<std::uint8_t>(this->m_Value) >> 4;
        }

        force_inline std::size_t GetInternetHeaderLength() const
        {
            return static_cast<std::uint8_t>(this->m_Value) & 0xf;
        }

        std::byte m_Value;
    };

    [[maybe_unused]] std::ostream& operator<<(std::ostream& iOStream, const VersionWithInternetHeaderLength& iVersionWithInternetHeaderLength)
    {
        return iOStream << "[Version=" << iVersionWithInternetHeaderLength.GetVersion() << " ; InternetHeaderLength=" << iVersionWithInternetHeaderLength.GetInternetHeaderLength() << "]";
    }

    enum class IPProtocol : std::uint8_t
    {
        TCP = 0x06,
        UDP = 0x11
    };

    // clang-format off
    DECLARE_MESSAGE_LAYOUT_PACKED(IPv4Header, 20,
        (Property<VersionWithInternetHeaderLength>, VersionWithInternetHeaderLength, VersionWithIHL)
        (ByteFillerProperty<1>, DSCPWithECN)
        (SwapEndiannessProperty<std::uint16_t>, TotalLength)
        (SwapEndiannessProperty<std::uint16_t>, Identification)
        (ByteFillerProperty<2>, FlagsWithFragmentOffset)
        (Property<std::uint8_t>, TimeToLive, TTL)
        (Property<IPProtocol>, Protocol)
        (SwapEndiannessProperty<std::uint16_t>, HeaderChecksum)
        (SwapEndiannessProperty<std::uint32_t>, SourceIPAddress)
        (SwapEndiannessProperty<std::uint32_t>, DestinationIPAddress));
    // clang-format on

    struct IPv4Header : public IPv4HeaderLayout
    {
        template <typename T>
        force_inline const T* GetPayload() const
        {
            return reinterpret_cast<const T*>(reinterpret_cast<const std::byte*>(this) + this->GetVersionWithInternetHeaderLength().Get().GetInternetHeaderLength() * sizeof(std::uint32_t));
        }

        [[nodiscard]] std::string GetFormattedDestinationAddress() const;
        [[nodiscard]] std::string GetFormattedSourceAddress() const;
    };
}
