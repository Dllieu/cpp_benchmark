#include <network/ipv4_header.h>

namespace
{
    std::string FormatIPv4Address(std::uint32_t iIPv4Address)
    {
        std::string result;

        for (std::size_t i = 0; i < 4; ++i)
        {
            result += std::to_string((iIPv4Address >> (i * 8)) & 0xff);

            if (i < 3)
            {
                result += '.';
            }
        }

        return result;
    }
}

namespace experimental
{
    std::string IPv4Header::GetFormattedDestinationAddress() const
    {
        return FormatIPv4Address(this->GetDestinationIPAddress().Get());
    }

    std::string IPv4Header::GetFormattedSourceAddress() const
    {
        return FormatIPv4Address(this->GetSourceIPAddress().Get());
    }
}
