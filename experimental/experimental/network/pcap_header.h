#pragma once

#include <messages/message.h>
#include <properties/array_variable_size_property.h>
#include <properties/property.h>
#include <tuple>
#include <utils/macros.h>

namespace experimental
{
    enum class LinkLayerType : std::uint32_t
    {
        Ethernet = 1,
    };

    // clang-format off
    DECLARE_MESSAGE_LAYOUT_PACKED(PcapHeader, 24,
        (Property<std::uint32_t>, MagicNumber)
        (Property<std::uint16_t>, MajorVersion)
        (Property<std::uint16_t>, MinorVersion)
        (Property<std::int32_t>, TimezoneOffset)
        (Property<std::uint32_t>, TimestampAccuracy)
        (Property<std::uint32_t>, SnapshotLength)
        (Property<LinkLayerType>, LinkLayerType));
    // clang-format on

    // clang-format off
    DECLARE_MESSAGE_LAYOUT_PACKED(RecordHeader, 16,
        (Property<std::uint32_t>, TimestampSeconds)
        (Property<std::uint32_t>, TimestampMicroseconds)
        (Property<std::uint32_t>, ByteRecordedInFileLength, Length)
        (Property<std::uint32_t>, OriginLength));
    // clang-format on

    struct Pcap
    {
        force_inline static std::tuple<const PcapHeaderLayout*, ArrayVariableSizeProperty<RecordHeaderLayout, false>> ReadPackets(const std::byte* iPcapBuffer, std::size_t iPcapTotalLength)
        {
            return std::make_tuple(reinterpret_cast<const PcapHeaderLayout*>(iPcapBuffer), ArrayVariableSizeProperty<RecordHeaderLayout, false>(iPcapBuffer + sizeof(PcapHeaderLayout), iPcapTotalLength - sizeof(PcapHeaderLayout)));
        }
    };
}
