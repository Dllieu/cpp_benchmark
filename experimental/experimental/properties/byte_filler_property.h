#pragma once

#include <cstddef>
#include <iostream>
#include <properties/property.h>

namespace experimental
{
    template <std::size_t N>
    struct ByteFillerProperty : public Property<std::byte, N>
    {
    };

    template <std::size_t N>
    [[maybe_unused]] std::ostream& operator<<(std::ostream& iOStream, const ByteFillerProperty<N>& /*iByteFillerProperty*/)
    {
        return iOStream;
    }
}
