#include <climits>
#include <gtest/gtest.h>
#include <utility>

namespace
{
    static_assert(8 == CHAR_BIT);

    template <class T, std::size_t... N>
    constexpr T bit_swap_impl(T i, std::index_sequence<N...> /* unused */)
    {
        return (((i >> N * CHAR_BIT & std::uint8_t(-1)) << (sizeof(T) - 1 - N) * CHAR_BIT) | ...);
    }

    template <class T, class U = std::make_unsigned_t<T>>
    constexpr U bit_swap(T i)
    {
        return bit_swap_impl<U>(i, std::make_index_sequence<sizeof(T)>{});
    }
}

TEST(BitManipulationTest, SwapEndianness) // NOLINT
{
    static_assert(0x3412u == bit_swap<std::uint16_t>(0x1234u));
    static_assert(0xefcdab8967452301ULL == bit_swap<std::uint64_t>(0x0123456789abcdefULL));
}
