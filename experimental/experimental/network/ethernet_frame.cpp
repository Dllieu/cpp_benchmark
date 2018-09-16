#include <iomanip>
#include <network/ethernet_frame.h>
#include <sstream>

namespace
{
    template <typename Buffer>
    static std::string GetMACAddress(const Buffer& iBuffer)
    {
        std::stringstream ss;
        bool useSeparator = false;

        for (std::uint8_t c : iBuffer)
        {
            ss << (useSeparator ? ":" : "");
            useSeparator = true;

            ss << std::setfill('0') << std::setw(2) << std::hex << static_cast<std::int32_t>(c);
        }

        return ss.str();
    }
}

namespace experimental
{
    std::string EthernetFrame::GetDestinationMACAddress() const
    {
        return ::GetMACAddress(this->GetDestinationAddress().Get());
    }

    std::string EthernetFrame::GetSourceMACAddress() const
    {
        return ::GetMACAddress(this->GetSourceAddress().Get());
    }
}
