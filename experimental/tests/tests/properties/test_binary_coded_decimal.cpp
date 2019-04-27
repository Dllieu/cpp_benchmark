#include <gtest/gtest.h>
#include <properties/binary_coded_decimal_property.h>

TEST(BinaryCodedDecimalTest, Packed_Default) // NOLINT
{
    std::array<std::uint8_t, 1> a{ 0b10010001 };
    auto p = reinterpret_cast<const experimental::BinaryCodedDecimal<a.size()>*>(a.data());

    EXPECT_EQ(91u, p->Get());
}

TEST(BinaryCodedDecimalTest, Packed_Default2) // NOLINT
{
    std::array<std::uint8_t, 5> a{ 0b00000001, 0b00100011, 0b01000101, 0b01100111, 0b10001001 };
    auto p = reinterpret_cast<const experimental::BinaryCodedDecimal<a.size()>*>(a.data());

    EXPECT_EQ(123456789u, p->Get());
}
