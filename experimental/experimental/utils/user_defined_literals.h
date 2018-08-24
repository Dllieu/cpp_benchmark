#pragma once

#include <utils/macros.h>
#include <cstddef>

// Global namespace is intended
constexpr force_inline std::size_t operator""_AlphaNumeric(const char* iString, std::size_t iSize)
{
    std::size_t result = 0;

    for (std::size_t i = 0; i < iSize; ++i)
    {
        result = (result << 8) | iString[i];
    }

    return result;
}

constexpr force_inline auto operator""_KB(unsigned long long int iValue)
{
    return iValue * 1'024;
}

constexpr force_inline auto operator""_MB(unsigned long long int iValue)
{
    return iValue * 1'024_KB;
}
