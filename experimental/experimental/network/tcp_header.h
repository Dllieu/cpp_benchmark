#pragma once

#include <messages/message.h>
#include <properties/property.h>
#include <properties/swap_endianness_property.h>
#include <utils/macros.h>

namespace experimental
{
    struct DataOffSetWithFlags
    {
        force_inline std::size_t GetDataOffset() const
        {
            return this->m_DataOffsetWithNS >> 4;
        }

        bool GetNS() const;
        bool GetCWR() const;
        bool GetECE() const;
        bool GetURG() const;
        bool GetACK() const;
        bool GetPSH() const;
        bool GetRST() const;
        bool GetSYN() const;
        bool GetFIN() const;

        std::uint8_t m_DataOffsetWithNS;
        std::uint8_t m_ControlBits;
    };

    [[maybe_unused]] inline std::ostream& operator<<(std::ostream& iOStream, const DataOffSetWithFlags& iDataOffSetWithFlags)
    {
        return iOStream << "[DataOffset=" << iDataOffSetWithFlags.GetDataOffset() << " | NS=" << iDataOffSetWithFlags.GetNS() << " | CWR=" << iDataOffSetWithFlags.GetCWR() << " | ECE=" << iDataOffSetWithFlags.GetECE()
                        << " | URG=" << iDataOffSetWithFlags.GetURG() << " | ACK=" << iDataOffSetWithFlags.GetACK() << " | PSH=" << iDataOffSetWithFlags.GetPSH() << " | RST=" << iDataOffSetWithFlags.GetRST()
                        << " | SYN=" << iDataOffSetWithFlags.GetSYN() << " | FIN=" << iDataOffSetWithFlags.GetFIN() << "]";
    }

    // clang-format off
    DECLARE_MESSAGE_LAYOUT_PACKED(TCPHeader, 20,
        (SwapEndiannessProperty<std::uint16_t>, SourcePort)
        (SwapEndiannessProperty<std::uint16_t>, DestinationPort)
        (SwapEndiannessProperty<std::uint32_t>, SequenceNumber)
        (SwapEndiannessProperty<std::uint32_t>, AcknowledgmentNumber)
        (Property<DataOffSetWithFlags>, DataOffSetWithFlags)
        (SwapEndiannessProperty<std::uint16_t>, WindowSize)
        (SwapEndiannessProperty<std::uint16_t>, CheckSum)
        (SwapEndiannessProperty<std::uint16_t>, UrgentPointer));
    // clang-format on

    struct TCPHeader : public TCPHeaderLayout
    {
        force_inline const std::byte* GetPayload() const
        {
            return reinterpret_cast<const std::byte*>(this) + this->GetDataOffSetWithFlags().Get().GetDataOffset() * sizeof(std::uint32_t);
        }
    };
}
