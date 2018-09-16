#include <climits>
#include <cstddef>
#include <fstream>
#include <gtest/gtest.h>
#include <network/ethernet_frame.h>
#include <network/hex_dump.h>
#include <network/ipv4_header.h>
#include <network/pcap_header.h>
#include <network/tcp_header.h>
#include <network/udp_header.h>

using experimental::EthernetFrame;
using experimental::IPv4Header;
using experimental::Pcap;
using experimental::TCPHeaderLayout;
using experimental::UDPHeaderLayout;

namespace
{
    struct PcapTest : public ::testing::Test
    {
        PcapTest()
        {
            std::array<char, PATH_MAX> buffer{};
            std::size_t count = readlink("/proc/self/exe", buffer.data(), buffer.size());

            m_Directory = std::string(buffer.data(), count > 0 ? count : 0);
            m_Directory = m_Directory.substr(0, m_Directory.find_last_of('/'));
        }

        void LoadFile(const std::string& iFilename)
        {
            std::ifstream file(this->m_Directory + iFilename, std::ios::binary);
            EXPECT_TRUE(file.is_open());

            file.seekg(0, std::ios::end);
            this->m_RawData = std::vector<std::byte>(file.tellg());
            file.seekg(0, std::ios::beg);

            file.read(reinterpret_cast<char*>(this->m_RawData.data()), this->m_RawData.size());
        }

        void SanityCheckPcapHeader(const experimental::PcapHeaderLayout* iPcapHeader)
        {
            EXPECT_EQ(2u, iPcapHeader->GetMajorVersion());
            EXPECT_EQ(4u, iPcapHeader->GetMinorVersion());
            EXPECT_EQ(0, iPcapHeader->GetTimezoneOffset());
            EXPECT_EQ(0u, iPcapHeader->GetTimestampAccuracy());

            EXPECT_TRUE(experimental::LinkLayerType::Ethernet == iPcapHeader->GetLinkLayerType());
        }

        std::string m_Directory;
        std::vector<std::byte> m_RawData;
    };
}

TEST_F(PcapTest, IPv4_TCP) // NOLINT
{
    this->LoadFile("/tests/data/tcp.pcap");

    auto [pPcapHeader, recordHeaders] = Pcap::ReadPackets(this->m_RawData.data(), this->m_RawData.size()); // NOLINT
    this->SanityCheckPcapHeader(pPcapHeader);

    for (auto [pRecordHeader, pPacket, packetSize] : recordHeaders) // NOLINT
    {
        auto* pEthernetFrame = reinterpret_cast<const EthernetFrame*>(pPacket);

        EXPECT_EQ(experimental::EthernetType::IPv4, pEthernetFrame->GetEthernetType().Get());

        auto* pIPv4Header = pEthernetFrame->GetPayload<IPv4Header>();

        EXPECT_EQ(5u, pIPv4Header->GetVersionWithInternetHeaderLength().Get().GetInternetHeaderLength());
        EXPECT_EQ(4u, pIPv4Header->GetVersionWithInternetHeaderLength().Get().GetVersion());
        EXPECT_EQ(experimental::IPProtocol::TCP, pIPv4Header->GetProtocol());

        auto* pTCPHeader = pIPv4Header->GetPayload<TCPHeaderLayout>();

        EXPECT_EQ(8u, pTCPHeader->GetDataOffSetWithFlags().Get().GetDataOffset());
        EXPECT_EQ(761199867u, pTCPHeader->GetSequenceNumber().Get());
        EXPECT_EQ(363u, pTCPHeader->GetWindowSize().Get());
    }
}

TEST_F(PcapTest, IPv4_UDP) // NOLINT
{
    this->LoadFile("/tests/data/udp.pcap");

    auto [pPcapHeader, recordHeaders] = Pcap::ReadPackets(this->m_RawData.data(), this->m_RawData.size()); // NOLINT
    this->SanityCheckPcapHeader(pPcapHeader);

    for (auto [pRecordHeader, pPacket, packetSize] : recordHeaders) // NOLINT
    {
        auto* pEthernetFrame = reinterpret_cast<const EthernetFrame*>(pPacket);

        EXPECT_EQ(experimental::EthernetType::IPv4, pEthernetFrame->GetEthernetType().Get());

        auto* pIPv4Header = pEthernetFrame->GetPayload<IPv4Header>();

        EXPECT_EQ(5u, pIPv4Header->GetVersionWithInternetHeaderLength().Get().GetInternetHeaderLength());
        EXPECT_EQ(4u, pIPv4Header->GetVersionWithInternetHeaderLength().Get().GetVersion());
        EXPECT_EQ(experimental::IPProtocol::UDP, pIPv4Header->GetProtocol());

        [[maybe_unused]] auto* pUDPHeader = pIPv4Header->GetPayload<UDPHeaderLayout>();
        std::cout << (*pUDPHeader) << std::endl;
    }
}
