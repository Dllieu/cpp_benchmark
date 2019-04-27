#include <gtest/gtest.h>
#include <properties/binary_coded_decimal_property.h>

TEST(BinaryCodedDecimalPropertyTest, Packed_Default) // NOLINT
{
    std::array<std::uint8_t, 5> a{ 0b00000001, 0b00100011, 0b01000101, 0b01100111, 0b10001001 };
    auto p = reinterpret_cast<const experimental::BinaryCodedDecimalProperty<a.size()>*>(a.data());

    EXPECT_EQ(123456789u, p->Get());
}

TEST(BinaryCodedDecimalPropertyTest, Unpacked_Default) // NOLINT
{
    std::array<std::uint8_t, 2> a{ 0b00000001, 0b00000011 };
    auto p = reinterpret_cast<const experimental::BinaryCodedDecimalProperty<a.size(), experimental::BinaryCodedDecimalType::Unpacked>*>(a.data());

    EXPECT_EQ(13u, p->Get());
}
