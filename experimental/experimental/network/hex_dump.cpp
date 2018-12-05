#include <cctype>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <network/hex_dump.h>
#include <string>
#include <utils/macros.h>

namespace experimental
{
    void HexDump(const std::byte* iBuffer, std::size_t iLength)
    {
        constexpr const char HexCharacters[] = "0123456789abcdef";
        constexpr const std::size_t Step = 16;

        std::ios_base::fmtflags previousIOFlags(std::cout.flags());
        std::cout << "HexDump [Length=" << iLength << "]:" << std::endl;

        for (std::size_t i = 0; i < iLength; i += Step)
        {
            std::string hexValues(Step * 2, ' ');
            std::string asciiValues;

            for (std::size_t j = 0; j < Step && i + j < iLength; ++j)
            {
                char c = static_cast<char>(iBuffer[i + j]);

                hexValues[j * 2] = HexCharacters[(c & 0xf0) >> 4];
                hexValues[j * 2 + 1] = HexCharacters[c & 0x0f];

                asciiValues += 0 != std::isprint(c) ? c : '.';
            }

            std::cout << '\t' << "0x" << std::setfill('0') << std::setw(4) << std::hex << i << ":  ";

            for (std::size_t j = 0; j < Step * 2; j += 4)
            {
                std::cout << std::string_view(hexValues.data() + j, 4) << ' ';
            }

            std::cout << ' ' << asciiValues << std::endl;
        }

        std::cout.flags(previousIOFlags);
    }
}
